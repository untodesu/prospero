// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: sessions.hh; Created: Tue Nov 25 2025 00:03:22
// Description: Session management

#ifndef SERVER_SESSIONS_HH
#define SERVER_SESSIONS_HH 1
#pragma once

#include "core/aes256.hh"
#include "core/ed25519.hh"

struct AuthChallengeRequest;
struct AuthChallengeResult;
struct ChannelDefinition;
struct SystemMessage;
struct TextMessage;

struct Session final {
    ENetPeer* session_peer { nullptr };
    ed25519::seed_buffer challenge;
    std::uint64_t challenge_timestamp;
    aes256::context aes_context { nullptr };
    std::string username;
};

namespace sessions
{
extern std::vector<Session> vector;
extern std::unordered_map<std::string, Session*> username_map;
extern std::unordered_set<std::string> username_set;
} // namespace sessions

namespace sessions
{
void init(void);
void shutdown(void);
} // namespace sessions

namespace sessions
{
void create(ENetPeer* peer);
void remove(ENetPeer* peer);
void update(ENetPeer* peer, const ENetPacket* packet, std::uint32_t channel);
} // namespace sessions

namespace sessions
{
Session* lookup(ENetPeer* peer);
Session* lookup(const std::string& username);
} // namespace sessions

namespace sessions
{
void send_packet(Session* session, std::uint32_t channel, const AuthChallengeRequest& packet);
void send_packet(Session* session, std::uint32_t channel, const AuthChallengeResult& packet);
void send_packet(Session* session, std::uint32_t channel, const ChannelDefinition& packet);
void send_packet(Session* session, std::uint32_t channel, const SystemMessage& packet);
void send_packet(Session* session, std::uint32_t channel, const TextMessage& packet);
} // namespace sessions

namespace sessions
{
void broadcast_packet(const SystemMessage& packet, std::uint32_t channel);
void broadcast_packet(const TextMessage& packet, std::uint32_t channel);
} // namespace sessions

#endif
