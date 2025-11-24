// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: sessions.cc; Created: Tue Nov 25 2025 00:10:26
// Description: Session management

#include "server/precompiled.hh"

#include "server/sessions.hh"

#include "core/buffer.hh"
#include "core/crc64ecma.hh"
#include "core/protocol.hh"
#include "core/unixtime.hh"

#include "server/channels.hh"
#include "server/identity.hh"
#include "server/settings.hh"
#include "server/userlist.hh"

std::vector<std::shared_ptr<Session>> sessions::list;
std::unordered_map<ENetPeer*, std::shared_ptr<Session>> sessions::by_peer;
std::unordered_map<std::uint64_t, std::shared_ptr<Session>> sessions::by_user_id;
std::unordered_map<std::string, std::shared_ptr<Session>> sessions::by_username;
std::unordered_set<std::string> sessions::usernames;

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

    while(sessions::usernames.contains(username)) {
        username = std::format("{}{}", desired_username, suffix);
        suffix += 1U;
    }

    return username;
}

Session::Session(ENetPeer* peer) : m_peer(peer)
{
    m_authenticated = false;
    m_challenge.fill(static_cast<std::byte>(0));
    m_challenge_timestamp = UINT64_MAX;
    m_public_key.fill(static_cast<std::byte>(0));
    m_user_id = UINT64_MAX;
}

void Session::challenge(void)
{
    core::ed25519::generate_seed(m_challenge);
    m_challenge_timestamp = core::unix_milliseconds();

    core::AuthChallengeRequest packet;
    packet.server_pkey = identity::public_key;
    packet.challenge_data = m_challenge;
    packet.challenge_timestamp = m_challenge_timestamp;

    send(packet);
}

bool Session::authenticate(const core::AuthChallengeResponse& packet)
{
    if(m_authenticated) {
        // Second authentication attempt; assume something
        // fishy is going on and reject the attempt
        return false;
    }

    auto current_time = core::unix_milliseconds();
    auto elapsed_time = current_time - m_challenge_timestamp;

    if(elapsed_time > settings::auth::timeout_ms) {
        core::AuthChallengeResult response;
        response.auth_status = core::AUTH_TIME;
        response.assigned_user_id = UINT64_MAX;
        response.assigned_username.clear();

        send(response);

        return false;
    }

    if(!core::ed25519::verify(packet.client_pkey, m_challenge, packet.signature)) {
        core::AuthChallengeResult response;
        response.auth_status = core::AUTH_CRED;
        response.assigned_user_id = UINT64_MAX;
        response.assigned_username.clear();

        send(response);

        return false;
    }

    if(!userlist::lookup(packet.client_pkey)) {
        core::AuthChallengeResult response;
        response.auth_status = core::AUTH_UNREC;
        response.assigned_user_id = UINT64_MAX;
        response.assigned_username.clear();

        send(response);

        return false;
    }

    m_public_key = packet.client_pkey;
    m_username = generate_username(packet.desired_username);
    m_user_id = core::crc64ecma(m_public_key);

    LOG_INFO("authenticated session user_id={}, username={}", m_user_id, m_username);

    core::ed25519::exch_buffer shared_secret;
    core::ed25519::generate_exch(identity::private_key, m_public_key, shared_secret);
    core::aes256::create(m_aes_context, shared_secret);

    core::AuthChallengeResult response;
    response.auth_status = core::AUTH_OK;
    response.assigned_user_id = m_user_id;
    response.assigned_username = m_username;

    send(response);

    m_authenticated = true;

    for(const auto& channel : channels::list) {
        core::ChannelDefinition packet;
        packet.channel_id = channel->channel_id();
        packet.channel_name = channel->name();

        send(packet);
    }

    sessions::sysmsg_inform(std::format("{} has connected", m_username));

    return true;
}

void Session::join_channel(std::uint64_t channel_id)
{
    m_channels.insert(channel_id);
}

void Session::leave_channel(std::uint64_t channel_id)
{
    m_channels.erase(channel_id);
}

bool Session::is_in_channel(std::uint64_t channel_id) const
{
    return m_channels.contains(channel_id);
}

void Session::send(const core::AuthChallengeRequest& packet)
{
    thread_local core::WriteBuffer buffer;

    buffer.reset();
    buffer.write<std::uint32_t>(static_cast<std::uint32_t>(core::PacketType::AuthChallengeRequest));

    core::AuthChallengeRequest::serialize(buffer, packet);

    enet_peer_send(m_peer, 0, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Session::send(const core::AuthChallengeResult& packet)
{
    thread_local core::WriteBuffer buffer;

    buffer.reset();
    buffer.write<std::uint32_t>(static_cast<std::uint32_t>(core::PacketType::AuthChallengeResult));

    core::AuthChallengeResult::serialize(buffer, packet);

    enet_peer_send(m_peer, 0, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Session::send(const core::ChannelDefinition& packet)
{
    assert(m_authenticated);
    assert(m_aes_context);

    thread_local core::WriteBuffer buffer;

    buffer.reset();
    buffer.write<std::uint32_t>(static_cast<std::uint32_t>(core::PacketType::ChannelDefinition));

    core::ChannelDefinition::serialize(m_aes_context, buffer, packet);

    enet_peer_send(m_peer, 0, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Session::send(const core::ChannelUserJoin& packet)
{
    assert(m_authenticated);
    assert(m_aes_context);

    thread_local core::WriteBuffer buffer;

    buffer.reset();
    buffer.write<std::uint32_t>(static_cast<std::uint32_t>(core::PacketType::ChannelUserJoin));

    core::ChannelUserJoin::serialize(m_aes_context, buffer, packet);

    enet_peer_send(m_peer, 0, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Session::send(const core::ChannelUserLeave& packet)
{
    assert(m_authenticated);
    assert(m_aes_context);

    thread_local core::WriteBuffer buffer;

    buffer.reset();
    buffer.write<std::uint32_t>(static_cast<std::uint32_t>(core::PacketType::ChannelUserLeave));

    core::ChannelUserLeave::serialize(m_aes_context, buffer, packet);

    enet_peer_send(m_peer, 0, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Session::send(const core::SystemMessage& packet)
{
    assert(m_authenticated);
    assert(m_aes_context);

    thread_local core::WriteBuffer buffer;

    buffer.reset();
    buffer.write<std::uint32_t>(static_cast<std::uint32_t>(core::PacketType::SystemMessage));

    core::SystemMessage::serialize(m_aes_context, buffer, packet);

    enet_peer_send(m_peer, 0, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Session::send(const core::TextMessage& packet)
{
    assert(m_authenticated);
    assert(m_aes_context);

    thread_local core::WriteBuffer buffer;

    buffer.reset();
    buffer.write<std::uint32_t>(static_cast<std::uint32_t>(core::PacketType::TextMessage));

    core::TextMessage::serialize(m_aes_context, buffer, packet);

    enet_peer_send(m_peer, 0, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void sessions::init(void)
{
    list.clear();
    by_peer.clear();
    by_user_id.clear();
    by_username.clear();
    usernames.clear();

    settings::auth::timeout_ms = std::clamp<unsigned long>(settings::auth::timeout_ms, 500U, 600000U);
}

void sessions::shutdown(void)
{
    list.clear();
    by_peer.clear();
    by_user_id.clear();
    by_username.clear();
    usernames.clear();
}

std::shared_ptr<Session> sessions::lookup(ENetPeer* peer)
{
    auto it = by_peer.find(peer);
    if(it == by_peer.cend())
        return nullptr;
    return it->second;
}

std::shared_ptr<Session> sessions::lookup(std::uint64_t user_id)
{
    auto it = by_user_id.find(user_id);
    if(it == by_user_id.cend())
        return nullptr;
    return it->second;
}

std::shared_ptr<Session> sessions::lookup(const std::string& username)
{
    auto it = by_username.find(username);
    if(it == by_username.cend())
        return nullptr;
    return it->second;
}

void sessions::create(ENetPeer* peer)
{
    assert(0 == by_peer.count(peer));

    auto session = std::make_shared<Session>(peer);

    session->challenge();

    list.push_back(session);

    by_peer[peer] = session;
}

void sessions::update(ENetPeer* peer, const ENetPacket* packet)
{
    assert(by_peer.contains(peer));

    auto session = by_peer[peer];
    auto context = session->aes_context();

    thread_local core::ReadBuffer buffer;
    thread_local core::AuthChallengeResponse auth_challenge_response;
    thread_local core::ChannelUserJoin channel_user_join;
    thread_local core::ChannelUserLeave channel_user_leave;
    thread_local core::TextMessage text_message;

    buffer.reset(packet->data, packet->dataLength);

    auto packet_type = static_cast<core::PacketType>(buffer.read<std::uint32_t>());

    if(packet_type == core::PacketType::AuthChallengeResponse) {
        if(context) {
            // Assume the client tries to do something
            // fishy if it sends multiple auth response
            // packets; we only need it once a connection
            remove(peer);
            return;
        }

        core::AuthChallengeResponse::deserialize(buffer, auth_challenge_response);

        if(!session->authenticate(auth_challenge_response)) {
            remove(peer);
            return;
        }

        return;
    }

    if(context == nullptr) {
        // Any packet other than an auth response
        // is encrypted and we can't decrypt it yet
        remove(peer);
        return;
    }

    switch(packet_type) {
        case core::PacketType::ChannelUserJoin:
            core::ChannelUserJoin::deserialize(context, buffer, channel_user_join);
            channels::session_join(session, channel_user_join.channel_id);
            break;

        case core::PacketType::ChannelUserLeave:
            core::ChannelUserLeave::deserialize(context, buffer, channel_user_leave);
            channels::session_leave(session, channel_user_leave.channel_id);
            break;

        case core::PacketType::TextMessage:
            core::TextMessage::deserialize(context, buffer, text_message);
            text_message.timestamp = core::unix_milliseconds();
            text_message.user_id = session->user_id();
            channels::broadcast(session, text_message);
            break;

        default:
            // Whatever the client just sent to us
            // is not recognized as a valid server-bound
            // packet; drop the session to be safe
            remove(peer);
            break;
    }
}

void sessions::remove(ENetPeer* peer)
{
    if(by_peer.contains(peer)) {
        auto session = by_peer[peer];
        auto context = session->aes_context();

        if(context) {
            channels::session_leave_all(session);

            by_user_id.erase(session->user_id());
            by_username.erase(session->username());
        }

        list.erase(std::ranges::find(list, session));
        by_peer.erase(peer);

        sessions::sysmsg_inform(std::format("{} has disconnected", session->username()));
    }
}

void sessions::sysmsg_inform(std::string_view message)
{
    thread_local core::SystemMessage packet;

    packet.severity = core::SEVR_INFORM;
    packet.timestamp = core::unix_milliseconds();
    packet.message = std::string(message);

    for(const auto& session : list) {
        if(session->aes_context()) {
            session->send(packet);
        }
    }
}

void sessions::sysmsg_warning(std::string_view message)
{
    thread_local core::SystemMessage packet;

    packet.severity = core::SEVR_WARNING;
    packet.timestamp = core::unix_milliseconds();
    packet.message = std::string(message);

    for(const auto& session : list) {
        if(session->aes_context()) {
            session->send(packet);
        }
    }
}

void sessions::sysmsg_critical(std::string_view message)
{
    thread_local core::SystemMessage packet;

    packet.severity = core::SEVR_CRITICAL;
    packet.timestamp = core::unix_milliseconds();
    packet.message = std::string(message);

    for(const auto& session : list) {
        if(session->aes_context()) {
            session->send(packet);
        }
    }
}

void sessions::broadcast(const core::ChannelUserJoin& packet, ENetPeer* exclude)
{
    for(const auto& session : list) {
        if(exclude == session->peer()) {
            continue;
        }

        if(session->aes_context()) {
            session->send(packet);
        }
    }
}

void sessions::broadcast(const core::ChannelUserLeave& packet, ENetPeer* exclude)
{
    for(const auto& session : list) {
        if(exclude == session->peer()) {
            continue;
        }

        if(session->aes_context()) {
            session->send(packet);
        }
    }
}
