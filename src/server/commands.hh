// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: commands.hh; Created: Thu Nov 27 2025 12:56:49
// Description: Maintenance commands

#ifndef SERVER_COMMANDS_HH
#define SERVER_COMMANDS_HH 1
#pragma once

struct Session;

using CommandHandler = void (*)(Session* sender, const std::vector<std::string_view>& arguments);

struct Command final {
    std::uint32_t permission {};
    std::string instructions;
    CommandHandler handler {};
};

namespace commands
{
extern std::unordered_map<std::string, Command> map;
} // namespace commands

namespace commands
{
void add(std::string_view name, const Command& command);
} // namespace commands

namespace commands
{
void exec(Session* sender, std::string_view name, const std::vector<std::string_view>& arguments);
void exec(Session* sender, std::string_view command);
} // namespace commands

#endif
