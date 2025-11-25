// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: channels.cc; Created: Mon Nov 24 2025 23:59:04
// Description: Channel management

#include "server/precompiled.hh"

#include "server/channels.hh"

#include "core/config.hh"
#include "core/exception.hh"

std::array<std::shared_ptr<Channel>, PROTOCOL_MAXCHAN> channels::array;

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

    if(entries.size() > PROTOCOL_MAXCHAN) {
        entries.resize(PROTOCOL_MAXCHAN);
    }

    if(entries.empty()) {
        throw core::runtime_error("no channels defined in {}", channels_directory.string());
    }

    array.fill(nullptr);

    Config config;

    for(std::size_t i = 0U; i < entries.size(); ++i) {
        assert(i < PROTOCOL_MAXCHAN);

        config.clear();

        if(config.read(entries[i])) {
            auto channel = std::make_shared<Channel>();
            channel->name = config.value<std::string_view>("name", std::format("channel{}", i));
            array[i] = channel;
        }
        else {
            throw core::runtime_error("failed to read channel configuration from {}", entries[i].string());
        }
    }
}

void channels::shutdown(void)
{
    array.fill(nullptr);
}
