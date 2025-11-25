// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: settings.hh; Created: Mon Nov 24 2025 23:24:40
// Description: Server settings

#ifndef SERVER_SETTINGS_HH
#define SERVER_SETTINGS_HH 1
#pragma once

namespace settings::host
{
extern unsigned short udp_port;
extern unsigned int max_peers;
} // namespace settings::host

namespace settings::auth
{
extern unsigned long timeout_ms;
} // namespace settings::auth

namespace settings
{
void init(const std::filesystem::path& config_directory);
void shutdown(void);
} // namespace settings

#endif
