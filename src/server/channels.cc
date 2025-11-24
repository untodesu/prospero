// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: channels.cc; Created: Mon Nov 24 2025 23:59:04
// Description: Server-side channel definitions

#include "server/precompiled.hh"

#include "server/channels.hh"

#include "core/config.hh"
#include "core/crc64ecma.hh"
#include "core/exception.hh"
#include "core/protocol.hh"
#include "core/string.hh"

#include "server/sessions.hh"

std::vector<std::shared_ptr<Channel>> channels::list;
std::unordered_map<std::uint64_t, std::shared_ptr<Channel>> channels::by_id;
std::unordered_map<std::string, std::shared_ptr<Channel>> channels::by_name;
std::unordered_set<std::string> channels::names;

Channel::Channel(const std::filesystem::path& config_filepath)
{
    core::Config config;

    if(!config.read(config_filepath)) {
        throw core::invalid_argument("{}: cannot read channel configuration file", config_filepath.string());
    }

    m_name = config.value<std::string_view>("name");

    if(core::is_whitespace(m_name) || core::has_whitespace(m_name)) {
        throw core::invalid_argument("{}: invalid channel name", config_filepath.string());
    }

    m_channel_id = core::crc64ecma(m_name);
}

void Channel::add_session(std::shared_ptr<Session> session)
{
    assert(session);

    if(session->is_in_channel(m_channel_id)) {
        return; // already in channel
    }

    m_sessions.insert(session);

    session->join_channel(m_channel_id);

    core::ChannelUserJoin packet;
    packet.user_id = session->user_id();
    packet.channel_id = m_channel_id;
    packet.username = session->username();

    broadcast(packet);
}

void Channel::remove_session(std::shared_ptr<Session> session)
{
    m_sessions.erase(session);

    if(!session->is_in_channel(m_channel_id)) {
        return; // not in channel
    }

    m_sessions.erase(session);

    session->leave_channel(m_channel_id);

    core::ChannelUserLeave packet;
    packet.user_id = session->user_id();
    packet.channel_id = m_channel_id;

    broadcast(packet);
}

bool Channel::has_session(std::shared_ptr<Session> session) const
{
    return m_sessions.contains(session);
}

void Channel::broadcast(const core::ChannelUserJoin& packet, ENetPeer* exclude)
{
    for(const auto& session : m_sessions) {
        if(exclude == session->peer()) {
            continue;
        }

        if(session->aes_context()) {
            session->send(packet);
        }
    }
}

void Channel::broadcast(const core::ChannelUserLeave& packet, ENetPeer* exclude)
{
    for(const auto& session : m_sessions) {
        if(exclude == session->peer()) {
            continue;
        }

        if(session->aes_context()) {
            session->send(packet);
        }
    }
}

void Channel::broadcast(const core::SystemMessage& packet, ENetPeer* exclude)
{
    for(const auto& session : m_sessions) {
        if(exclude == session->peer()) {
            continue;
        }

        if(session->aes_context()) {
            session->send(packet);
        }
    }
}

void Channel::broadcast(const core::TextMessage& packet, ENetPeer* exclude)
{
    for(const auto& session : m_sessions) {
        if(exclude == session->peer()) {
            continue;
        }

        if(session->aes_context()) {
            session->send(packet);
        }
    }
}

void channels::init(const std::filesystem::path& config_directory)
{
    std::filesystem::path channels_directory(config_directory / "channels.d");
    std::filesystem::create_directories(channels_directory);

    std::vector<std::filesystem::path> entries;

    for(const auto& entry : std::filesystem::directory_iterator(channels_directory)) {
        if(entry.is_regular_file()) {
            entries.push_back(entry.path());
        }
    }

    std::sort(entries.begin(), entries.end());

    for(const auto& entry : entries) {
        auto channel = std::make_shared<Channel>(entry);

        LOG_INFO("adding channel: {} id={}", channel->name(), channel->channel_id());

        list.push_back(channel);

        by_id[channel->channel_id()] = channel;
        by_name[channel->name()] = channel;

        names.insert(channel->name());
    }

    if(list.empty()) {
        throw core::runtime_error("no channels defined in {}", channels_directory.string());
    }
}

void channels::shutdown(void)
{
    list.clear();
    by_id.clear();
    by_name.clear();
    names.clear();
}

std::shared_ptr<Channel> channels::lookup(std::uint64_t channel_id)
{
    auto it = by_id.find(channel_id);
    if(it == by_id.cend())
        return nullptr;
    return it->second;
}

std::shared_ptr<Channel> channels::lookup(const std::string& channel_name)
{
    auto it = by_name.find(channel_name);
    if(it == by_name.cend())
        return nullptr;
    return it->second;
}

void channels::session_join(const std::shared_ptr<Session>& session, std::uint64_t channel_id)
{
    assert(session);

    if(auto channel = lookup(channel_id)) {
        channel->add_session(session);
    }
}

void channels::session_leave(const std::shared_ptr<Session>& session, std::uint64_t channel_id)
{
    assert(session);

    if(auto channel = lookup(channel_id)) {
        channel->remove_session(session);
    }
}

void channels::session_leave_all(const std::shared_ptr<Session>& session)
{
    assert(session);

    for(const auto& channel : list) {
        if(channel->has_session(session)) {
            channel->remove_session(session);
        }
    }
}

void channels::broadcast(const std::shared_ptr<Session>& session, const core::TextMessage& packet)
{
    assert(session);
    assert(packet.user_id == session->user_id());

    if(auto channel = lookup(packet.channel_id)) {
        channel->broadcast(packet);
    }
}
