// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: sessions.hh; Created: Tue Nov 25 2025 00:03:22
// Description: Session management

#ifndef SERVER_SESSIONS_HH
#define SERVER_SESSIONS_HH 1
#pragma once

#include "core/aes256.hh"
#include "core/ed25519.hh"

struct AuthRequest;
struct AuthResult;
struct Notification;
struct BasicMessage;

struct Session final {
    ENetPeer* peer { nullptr };
    aes256::context aes_context { nullptr };
    ed25519::pkey_buffer public_key {};
    ed25519::seed_buffer challenge {};
    std::uint64_t auth_timestamp {};
    std::string away_message {};
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
void update(ENetPeer* peer, const ENetPacket* packet);
} // namespace sessions

namespace sessions
{
Session* lookup(ENetPeer* peer);
Session* lookup(const std::string& username);
Session* lookup(const ed25519::pkey_buffer& public_key);
} // namespace sessions

namespace sessions
{
void send_packet(Session* session, const AuthRequest& packet);
void send_packet(Session* session, const AuthResult& packet);
void send_packet(Session* session, const Notification& packet);
void send_packet(Session* session, const BasicMessage& packet);
} // namespace sessions

namespace sessions
{
void send_notification(Session* session, std::uint32_t type, std::string_view text_1, std::string_view text_2 = {});
} // namespace sessions

namespace sessions
{
void broadcast_packet(const Notification& packet);
void broadcast_packet(const BasicMessage& packet);
} // namespace sessions

namespace sessions
{
void broadcast_notification(std::uint32_t type, std::string_view text_1, std::string_view text_2 = {});
} // namespace sessions

#endif
