// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: sessions.hh; Created: Tue Nov 25 2025 00:03:22
// Description: Session management

#ifndef SERVER_SESSIONS_HH
#define SERVER_SESSIONS_HH 1
#pragma once

#include "core/aes256.hh"
#include "core/ed25519.hh"

namespace core
{
struct AuthChallengeRequest;
struct AuthChallengeResponse;
struct AuthChallengeResult;
struct ChannelDefinition;
struct ChannelUserJoin;
struct ChannelUserLeave;
struct SystemMessage;
struct TextMessage;
} // namespace core

class Session final {
public:
    explicit Session(ENetPeer* peer);

    constexpr ENetPeer* peer(void) const;
    constexpr core::aes256::context aes_context(void) const;

    constexpr std::uint64_t user_id(void) const;
    constexpr const std::string& username(void) const;

    void challenge(void);
    bool authenticate(const core::AuthChallengeResponse& packet);

    void join_channel(std::uint64_t channel_id);
    void leave_channel(std::uint64_t channel_id);
    bool is_in_channel(std::uint64_t channel_id) const;

    void send(const core::AuthChallengeRequest& packet);
    void send(const core::AuthChallengeResult& packet);
    void send(const core::ChannelDefinition& packet);
    void send(const core::ChannelUserJoin& packet);
    void send(const core::ChannelUserLeave& packet);
    void send(const core::SystemMessage& packet);
    void send(const core::TextMessage& packet);

private:
    ENetPeer* m_peer;
    bool m_authenticated;

    core::ed25519::seed_buffer m_challenge;
    std::uint64_t m_challenge_timestamp;

    core::ed25519::pkey_buffer m_public_key;
    std::uint64_t m_user_id;
    std::string m_username;

    core::aes256::context m_aes_context;

    std::unordered_set<std::uint64_t> m_channels;
};

namespace sessions
{
extern std::vector<std::shared_ptr<Session>> list;
extern std::unordered_map<ENetPeer*, std::shared_ptr<Session>> by_peer;
extern std::unordered_map<std::uint64_t, std::shared_ptr<Session>> by_user_id;
extern std::unordered_map<std::string, std::shared_ptr<Session>> by_username;
extern std::unordered_set<std::string> usernames;
} // namespace sessions

namespace sessions
{
void init(void);
void shutdown(void);
} // namespace sessions

namespace sessions
{
std::shared_ptr<Session> lookup(ENetPeer* peer);
std::shared_ptr<Session> lookup(std::uint64_t user_id);
std::shared_ptr<Session> lookup(const std::string& username);
} // namespace sessions

namespace sessions
{
void create(ENetPeer* peer);
void update(ENetPeer* peer, const ENetPacket* packet);
void remove(ENetPeer* peer);
} // namespace sessions

namespace sessions
{
void sysmsg_inform(std::string_view message);
void sysmsg_warning(std::string_view message);
void sysmsg_critical(std::string_view message);
} // namespace sessions

namespace sessions
{
void broadcast(const core::ChannelUserJoin& packet, ENetPeer* exclude = nullptr);
void broadcast(const core::ChannelUserLeave& packet, ENetPeer* exclude = nullptr);
} // namespace sessions

constexpr ENetPeer* Session::peer(void) const
{
    return m_peer;
}

constexpr core::aes256::context Session::aes_context(void) const
{
    return m_aes_context;
}

constexpr std::uint64_t Session::user_id(void) const
{
    return m_user_id;
}

constexpr const std::string& Session::username(void) const
{
    return m_username;
}

#endif // SERVER_SESSIONS_HH
