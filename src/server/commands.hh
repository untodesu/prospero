// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: commands.hh; Created: Thu Nov 27 2025 12:56:49
// Description: Maintenance commands

#ifndef SERVER_COMMANDS_HH
#define SERVER_COMMANDS_HH 1
#pragma once

struct Session;

enum class CommandGroup : unsigned int {
    General = 0U, ///< General commands available to all users
    IrcLike,      ///< General commands available to all users, except for IRC-like commands
    Admin,        ///< Administration commands, available to sessions with PERM_OPER and higher
    Root,         ///< Root commands, available to sessions with PERM_ROOT
    COUNT         ///< Number of command groups
};

constexpr static std::size_t NUM_COMMAND_GROUPS = static_cast<std::size_t>(CommandGroup::COUNT);

using CommandHandler = void (*)(Session* sender, const std::vector<std::string_view>& arguments);

namespace commands
{
extern std::array<std::unordered_map<std::string, std::pair<CommandHandler, std::string>>, NUM_COMMAND_GROUPS> groups;
} // namespace commands

namespace commands
{
void add(CommandGroup group, std::string_view name, CommandHandler handler, std::string_view usage = {});
} // namespace commands

namespace commands
{
void exec(Session* sender, std::string_view name, const std::vector<std::string_view>& arguments);
void exec(Session* sender, std::string_view command);
} // namespace commands

#endif
