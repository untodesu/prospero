// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: sessions.cc; Created: Tue Nov 25 2025 00:10:26
// Description: Session management

#include "server/precompiled.hh"

#include "server/sessions.hh"

#include "core/buffer.hh"
#include "core/protocol.hh"
#include "core/unixtime.hh"

#include "server/channels.hh"
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

    session->session_peer = nullptr;
    session->challenge.fill(static_cast<std::byte>(0));
    session->challenge_timestamp = UINT64_MAX;
    session->aes_context = nullptr;
    session->username.clear();
}

static std::string generate_username(std::string_view desired_username)
{
    std::string username;
    username.reserve(64U);

    for(std::size_t i = 0U; i < desired_username.size() && i < 64U; ++i) {
        auto is_valid_character = false;
        is_valid_character = is_valid_character || std::isalnum(desired_username[i]); // allow usernames like dreeslow123
        is_valid_character = is_valid_character || desired_username[i] == '_';        // allow usernames like johnny_dreeslow
        is_valid_character = is_valid_character || desired_username[i] == '-';        // allow usernames like johnny-dreeslow
        is_valid_character = is_valid_character || desired_username[i] == '.';        // allow usernames like johnny.dreeslow

        if(is_valid_character) {
            username.push_back(desired_username[i]);
        }
    }

    unsigned long suffix = 1U;

    while(sessions::username_set.contains(username)) {
        username = std::format("{}{}", desired_username, suffix);
        suffix += 1U;
    }

    return username;
}

static bool authenticate_session(Session* session, const AuthChallengeResponse& packet)
{
    if(session->aes_context) {
        // Second authentication attempt; assume something
        // fishy is going on and reject the attempt
        return false;
    }

    auto current_time = unixtime::milliseconds();
    auto elapsed_time = current_time - session->challenge_timestamp;

    if(elapsed_time > settings::auth::timeout_ms) {
        AuthChallengeResult response;
        response.status = AuthChallengeResult::E_TIME;
        response.username.clear();

        sessions::send_packet(session, PROTOCOL_AUTHCHAN, response);

        return false;
    }

    if(!ed25519::verify(packet.client_pkey, session->challenge, packet.signature)) {
        AuthChallengeResult response;
        response.status = AuthChallengeResult::E_CRED;
        response.username.clear();

        sessions::send_packet(session, PROTOCOL_AUTHCHAN, response);

        return false;
    }

    if(!userlist::lookup(packet.client_pkey)) {
        AuthChallengeResult response;
        response.status = AuthChallengeResult::E_UNREC;
        response.username.clear();

        sessions::send_packet(session, PROTOCOL_AUTHCHAN, response);

        return false;
    }

    session->username = generate_username(packet.username);

    LOG_INFO("authenticated session username={}", session->username);

    ed25519::exch_buffer shared_secret;
    ed25519::generate_exch(identity::private_key, packet.client_pkey, shared_secret);
    aes256::create(session->aes_context, shared_secret);

    AuthChallengeResult response;
    response.status = AuthChallengeResult::E_OK;
    response.username = session->username;

    sessions::send_packet(session, PROTOCOL_AUTHCHAN, response);

    for(std::uint32_t i = 0U; i < PROTOCOL_MAXCHAN; ++i) {
        if(auto channel = channels::array.at(i)) {
            ChannelDefinition packet;
            packet.name = channel->name;

            sessions::send_packet(session, i, packet);
        }
    }

    return true;
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
        if(vector[i].session_peer == nullptr) {
            reset_session_data(&vector[i]);

            vector[i].session_peer = peer;
            vector[i].challenge_timestamp = unixtime::milliseconds();
            ed25519::generate_seed(vector[i].challenge);

            peer->data = &vector[i];

            AuthChallengeRequest packet;
            packet.server_pkey = identity::public_key;
            packet.challenge_data = vector[i].challenge;
            packet.challenge_timestamp = vector[i].challenge_timestamp;
            packet.protocol_version = PROTOCOL_VERSION;

            send_packet(&vector[i], PROTOCOL_AUTHCHAN, packet);

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

            reset_session_data(session);
        }
    }
}

void sessions::update(ENetPeer* peer, const ENetPacket* packet, std::uint32_t channel)
{
    assert(peer);
    assert(packet);

    thread_local ReadBuffer buffer;
    thread_local AuthChallengeResponse auth_challenge_response;
    thread_local TextMessage text_message;

    if(auto session = lookup(peer)) {
        buffer.reset(packet->data, packet->dataLength);

        auto packet_type = buffer.read<std::uint32_t>();

        if(packet_type == AuthChallengeResponse::ID) {
            AuthChallengeResponse::deserialize(buffer, auth_challenge_response);

            auto authenticated = true;
            authenticated = authenticated && channel == PROTOCOL_AUTHCHAN;
            authenticated = authenticated && authenticate_session(session, auth_challenge_response);

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
                    text_message.timestamp = unixtime::milliseconds();
                    text_message.username = session->username;
                    broadcast_packet(text_message, channel);
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

void sessions::send_packet(Session* session, std::uint32_t channel, const AuthChallengeRequest& packet)
{
    assert(session);
    assert(channel == PROTOCOL_AUTHCHAN);

    thread_local WriteBuffer buffer;

    buffer.reset();
    buffer.write<std::uint32_t>(AuthChallengeRequest::ID);
    AuthChallengeRequest::serialize(buffer, packet);

    enet_peer_send(session->session_peer, channel, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void sessions::send_packet(Session* session, std::uint32_t channel, const AuthChallengeResult& packet)
{
    assert(session);
    assert(channel == PROTOCOL_AUTHCHAN);

    thread_local WriteBuffer buffer;

    buffer.reset();
    buffer.write<std::uint32_t>(AuthChallengeResult::ID);
    AuthChallengeResult::serialize(buffer, packet);

    enet_peer_send(session->session_peer, channel, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void sessions::send_packet(Session* session, std::uint32_t channel, const ChannelDefinition& packet)
{
    assert(session);
    assert(session->aes_context);
    assert(channel < PROTOCOL_MAXCHAN);

    thread_local WriteBuffer buffer;

    buffer.reset();
    buffer.write<std::uint32_t>(ChannelDefinition::ID);
    ChannelDefinition::serialize(session->aes_context, buffer, packet);

    enet_peer_send(session->session_peer, channel, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void sessions::send_packet(Session* session, std::uint32_t channel, const SystemMessage& packet)
{
    assert(session);
    assert(session->aes_context);
    assert(channel < PROTOCOL_MAXCHAN);

    thread_local WriteBuffer buffer;

    buffer.reset();
    buffer.write<std::uint32_t>(SystemMessage::ID);
    SystemMessage::serialize(session->aes_context, buffer, packet);

    enet_peer_send(session->session_peer, channel, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void sessions::send_packet(Session* session, std::uint32_t channel, const TextMessage& packet)
{
    assert(session);
    assert(session->aes_context);
    assert(channel < PROTOCOL_MAXCHAN);

    thread_local WriteBuffer buffer;

    buffer.reset();
    buffer.write<std::uint32_t>(TextMessage::ID);
    TextMessage::serialize(session->aes_context, buffer, packet);

    enet_peer_send(session->session_peer, channel, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void sessions::broadcast_packet(const SystemMessage& packet, std::uint32_t channel)
{
    for(auto& session : vector) {
        if(session.session_peer && session.aes_context) {
            send_packet(&session, channel, packet);
        }
    }
}

void sessions::broadcast_packet(const TextMessage& packet, std::uint32_t channel)
{
    for(auto& session : vector) {
        if(session.session_peer && session.aes_context) {
            send_packet(&session, channel, packet);
        }
    }
}
