// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: sessions.cc; Created: Tue Nov 25 2025 00:10:26
// Description: Session management

#include "server/precompiled.hh"

#include "server/sessions.hh"

#include "core/buffer.hh"
#include "core/protocol.hh"
#include "core/strtools.hh"
#include "core/unixtime.hh"
#include "core/version.hh"

#include "server/commands.hh"
#include "server/host.hh"
#include "server/identity.hh"
#include "server/settings.hh"
#include "server/userlist.hh"

std::vector<Session> sessions::vector;
std::unordered_map<std::string, Session*> sessions::username_map;
std::unordered_set<std::string> sessions::username_set;

static void reset_session_data(Session* session)
{
    if(session->aes_context) {
        aes256::destroy(session->aes_context);
    }

    session->peer = nullptr;
    session->aes_context = nullptr;
    session->challenge.fill(static_cast<std::byte>(0));
    session->auth_timestamp = UINT64_MAX;
    session->username.clear();
}

static std::string generate_username(std::string_view desired_username)
{
    std::string filtered_username;
    filtered_username.reserve(64U);

    for(std::size_t i = 0U; i < desired_username.size() && i < 64U; ++i) {
        auto is_valid_character = false;
        is_valid_character = is_valid_character || std::isalnum(desired_username[i]); // allow usernames like dreeslow123
        is_valid_character = is_valid_character || desired_username[i] == '_';        // allow usernames like johnny_dreeslow
        is_valid_character = is_valid_character || desired_username[i] == '-';        // allow usernames like johnny-dreeslow
        is_valid_character = is_valid_character || desired_username[i] == '.';        // allow usernames like johnny.dreeslow

        if(is_valid_character) {
            filtered_username.push_back(desired_username[i]);
        }
    }

    if(filtered_username.empty()) {
        filtered_username = "prosperoclient";
    }

    std::string username = filtered_username;
    unsigned long suffix = 1U;

    while(sessions::username_set.contains(username)) {
        username = std::format("{}{}", filtered_username, suffix);
        suffix += 1U;
    }

    return username;
}

static bool authenticate_session(Session* session, const AuthResponse& packet)
{
    if(session->aes_context) {
        // Second authentication attempt; assume something
        // fishy is going on and reject the attempt
        return false;
    }

    auto current_time = unixtime::milliseconds();
    auto elapsed_time = current_time - session->auth_timestamp;

    auto authenticated = true;
    authenticated = authenticated && elapsed_time <= settings::auth::timeout_ms;
    authenticated = authenticated && ed25519::verify(packet.public_key, session->challenge, packet.signature);
    authenticated = authenticated && userlist::lookup(packet.public_key);

    if(!settings::auth::allow_dopplegangers) {
        auto original_session = sessions::lookup(packet.public_key);

        if(original_session && original_session->aes_context) {
            authenticated = false;
        }
    }

    if(authenticated) {
        session->public_key = packet.public_key;
        session->username = generate_username(packet.desired_username);

        LOG_INFO("assigned username={} to authenticated session", session->username);

        ed25519::exch_buffer shared_secret;
        ed25519::generate_exch(identity::private_key, packet.public_key, shared_secret);
        aes256::create(session->aes_context, shared_secret);

        AuthResult response;
        response.public_key = identity::public_key;
        response.assigned_username = session->username;
        sessions::send_packet(session, response);

        sessions::broadcast_notification(Notification::T_PEER_JOIN, session->username);
    }

    return authenticated;
}

static void handle_text_message(Session* session, const TextMessage& packet)
{
    assert(session);
    assert(session->aes_context);

    if(packet.message.starts_with('/')) {
        commands::exec(session, packet.message.substr(1U));
        return;
    }

    thread_local TextMessage text_message;
    text_message.timestamp = unixtime::milliseconds();
    text_message.username = session->username;
    text_message.message = packet.message;

    sessions::broadcast_packet(text_message);
}

void sessions::init(void)
{
    username_map.clear();
    username_set.clear();

    vector.resize(settings::host::max_peers);

    for(std::size_t i = 0U; i < vector.size(); ++i) {
        reset_session_data(&vector[i]);
    }

    settings::auth::timeout_ms = std::clamp<unsigned long>(settings::auth::timeout_ms, 500U, 600000U);
}

void sessions::shutdown(void)
{
    for(std::size_t i = 0U; i < vector.size(); ++i) {
        reset_session_data(&vector[i]);
    }

    vector.clear();
    username_map.clear();
    username_set.clear();
}

void sessions::create(ENetPeer* peer)
{
    assert(peer);

    for(std::size_t i = 0U; i < vector.size(); ++i) {
        if(vector[i].peer == nullptr) {
            reset_session_data(&vector[i]);

            vector[i].peer = peer;
            vector[i].auth_timestamp = unixtime::milliseconds();
            ed25519::generate_seed(vector[i].challenge);

            peer->data = &vector[i];

            AuthRequest packet;
            packet.version_major = static_cast<std::uint32_t>(version::major);
            packet.version_minor = static_cast<std::uint32_t>(version::minor);
            packet.version_patch = static_cast<std::uint32_t>(version::patch);
            packet.auth_timestamp = vector[i].auth_timestamp;
            packet.challenge = vector[i].challenge;
            send_packet(&vector[i], packet);

            return;
        }
    }

    enet_peer_disconnect(peer, 0U);
}

void sessions::remove(ENetPeer* peer)
{
    assert(peer);

    if(auto session = lookup(peer)) {
        if(session->aes_context) {
            username_map.erase(session->username);
            username_set.erase(session->username);

            sessions::broadcast_notification(Notification::T_PEER_LEFT, session->username);

            reset_session_data(session);
        }
    }
}

void sessions::update(ENetPeer* peer, const ENetPacket* packet)
{
    assert(peer);
    assert(packet);

    thread_local ReadBuffer buffer;
    thread_local AuthResponse auth_response;
    thread_local TextMessage text_message;

    if(auto session = lookup(peer)) {
        buffer.reset(packet->data, packet->dataLength);

        auto packet_type = buffer.read<std::uint32_t>();

        if(packet_type == AuthResponse::ID) {
            AuthResponse::deserialize(buffer, auth_response);

            auto authenticated = authenticate_session(session, auth_response);

            for(int i = 0; i < 10; ++i) {
                // This ensures the packets get transmitted
                // because for some reason enet_host_flush just
                // doesn't work here? PATCHME: this is dumb
                host::update();
            }

            if(authenticated) {
                username_map[session->username] = session;
                username_set.insert(session->username);
            }
            else {
                enet_peer_disconnect(peer, 0U);
            }

            return;
        }

        if(session->aes_context) {
            switch(packet_type) {
                case TextMessage::ID:
                    TextMessage::deserialize(session->aes_context, buffer, text_message);
                    handle_text_message(session, text_message);
                    break;
            }
        }
    }
}

Session* sessions::lookup(ENetPeer* peer)
{
    return reinterpret_cast<Session*>(peer->data);
}

Session* sessions::lookup(const std::string& username)
{
    auto it = username_map.find(username);
    if(it == username_map.cend())
        return nullptr;
    return it->second;
}

Session* sessions::lookup(const ed25519::pkey_buffer& public_key)
{
    for(auto& session : vector) {
        if(session.peer && session.public_key == public_key) {
            return &session;
        }
    }

    return nullptr;
}

void sessions::send_packet(Session* session, const AuthRequest& packet)
{
    assert(session);

    thread_local WriteBuffer buffer;

    buffer.reset();
    buffer.write<std::uint32_t>(AuthRequest::ID);
    AuthRequest::serialize(buffer, packet);

    enet_peer_send(session->peer, 0U, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void sessions::send_packet(Session* session, const AuthResult& packet)
{
    assert(session);

    thread_local WriteBuffer buffer;

    buffer.reset();
    buffer.write<std::uint32_t>(AuthResult::ID);
    AuthResult::serialize(buffer, packet);

    enet_peer_send(session->peer, 0U, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void sessions::send_packet(Session* session, const Notification& packet)
{
    assert(session);
    assert(session->aes_context);

    thread_local WriteBuffer buffer;

    buffer.reset();
    buffer.write<std::uint32_t>(Notification::ID);
    Notification::serialize(session->aes_context, buffer, packet);

    enet_peer_send(session->peer, 0U, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void sessions::send_packet(Session* session, const TextMessage& packet)
{
    assert(session);
    assert(session->aes_context);

    thread_local WriteBuffer buffer;

    buffer.reset();
    buffer.write<std::uint32_t>(TextMessage::ID);
    TextMessage::serialize(session->aes_context, buffer, packet);

    enet_peer_send(session->peer, 0U, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void sessions::send_notification(Session* session, std::uint32_t type, std::string_view text)
{
    assert(session);
    assert(session->aes_context);

    thread_local Notification packet;

    packet.timestamp = unixtime::milliseconds();
    packet.type = type;
    packet.text = text;

    send_packet(session, packet);
}

void sessions::broadcast_packet(const Notification& packet)
{
    for(auto& session : vector) {
        if(session.peer && session.aes_context) {
            send_packet(&session, packet);
        }
    }
}

void sessions::broadcast_packet(const TextMessage& packet)
{
    for(auto& session : vector) {
        if(session.peer && session.aes_context) {
            send_packet(&session, packet);
        }
    }
}

void sessions::broadcast_notification(std::uint32_t type, std::string_view text)
{
    thread_local Notification packet;

    packet.timestamp = unixtime::milliseconds();
    packet.type = type;
    packet.text = text;

    broadcast_packet(packet);
}
