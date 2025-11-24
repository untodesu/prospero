// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: channels.hh; Created: Mon Nov 24 2025 23:54:28
// Description: Channel management

#ifndef SERVER_CHANNELS_HH
#define SERVER_CHANNELS_HH 1
#pragma once

namespace core
{
struct ChannelUserJoin;
struct ChannelUserLeave;
struct SystemMessage;
struct TextMessage;
} // namespace core

class Session;
class Channel final {
public:
    explicit Channel(const std::filesystem::path& config_filepath);

    constexpr const std::string& name(void) const;
    constexpr std::uint64_t channel_id(void) const;

    void add_session(std::shared_ptr<Session> session);
    void remove_session(std::shared_ptr<Session> session);
    bool has_session(std::shared_ptr<Session> session) const;

    void broadcast(const core::ChannelUserJoin& packet, ENetPeer* exclude = nullptr);
    void broadcast(const core::ChannelUserLeave& packet, ENetPeer* exclude = nullptr);
    void broadcast(const core::SystemMessage& packet, ENetPeer* exclude = nullptr);
    void broadcast(const core::TextMessage& packet, ENetPeer* exclude = nullptr);

private:
    std::string m_name;
    std::uint64_t m_channel_id;
    std::unordered_set<std::shared_ptr<Session>> m_sessions;
};

namespace channels
{
extern std::vector<std::shared_ptr<Channel>> list;
extern std::unordered_map<std::uint64_t, std::shared_ptr<Channel>> by_id;
extern std::unordered_map<std::string, std::shared_ptr<Channel>> by_name;
extern std::unordered_set<std::string> names;
} // namespace channels

namespace channels
{
void init(const std::filesystem::path& config_directory);
void shutdown(void);
} // namespace channels

namespace channels
{
std::shared_ptr<Channel> lookup(std::uint64_t channel_id);
std::shared_ptr<Channel> lookup(const std::string& channel_name);
} // namespace channels

namespace channels
{
void session_join(const std::shared_ptr<Session>& session, std::uint64_t channel_id);
void session_leave(const std::shared_ptr<Session>& session, std::uint64_t channel_id);
void session_leave_all(const std::shared_ptr<Session>& session);
} // namespace channels

namespace channels
{
void broadcast(const std::shared_ptr<Session>& session, const core::TextMessage& packet);
} // namespace channels

constexpr const std::string& Channel::name(void) const
{
    return m_name;
}

constexpr std::uint64_t Channel::channel_id(void) const
{
    return m_channel_id;
}

#endif // SERVER_CHANNELS_HH
