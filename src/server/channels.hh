// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: channels.hh; Created: Mon Nov 24 2025 23:54:28
// Description: Channel management

#ifndef SERVER_CHANNELS_HH
#define SERVER_CHANNELS_HH 1
#pragma once

#include "core/protocol.hh"

struct Channel final {
    std::string name;
};

namespace channels
{
extern std::array<std::shared_ptr<Channel>, PROTOCOL_MAXCHAN> array;
} // namespace channels

namespace channels
{
void init(const std::filesystem::path& config_directory);
void shutdown(void);
} // namespace channels

#endif
