// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: commands.cc; Created: Thu Nov 27 2025 12:57:44
// Description: Maintenance commands

#include "server/precompiled.hh"

#include "server/commands.hh"

#include "core/exception.hh"
#include "core/protocol.hh"
#include "core/strtools.hh"

#include "server/sessions.hh"
#include "server/userlist.hh"

constexpr static std::array<std::uint32_t, NUM_COMMAND_GROUPS> GROUP_PERMISSIONS = {
    PERM_USER, ///< CommandGroup::General
    PERM_USER, ///< CommandGroup::IrcLike
    PERM_OPER, ///< CommandGroup::Admin
    PERM_ROOT, ///< CommandGroup::Root
};

std::array<std::unordered_map<std::string, std::pair<CommandHandler, std::string>>, NUM_COMMAND_GROUPS> commands::groups;

void commands::add(CommandGroup group, std::string_view name, CommandHandler handler, std::string_view usage)
{
    auto group_index = static_cast<std::size_t>(group);

    assert(group_index < NUM_COMMAND_GROUPS);
    assert(name.size());
    assert(handler);

    for(const auto& group : commands::groups) {
        assert(0 == group.count(std::string(name)));
    }

    groups[group_index].insert_or_assign(std::string(name), std::make_pair(handler, std::string(usage)));
}

void commands::exec(Session* sender, std::string_view name, const std::vector<std::string_view>& arguments)
{
    assert(sender);
    assert(sender->aes_context);
    assert(name.size());

    CommandHandler handler = nullptr;
    std::uint32_t required_permission = PERM_NULL;

    for(std::size_t i = 0U; i < NUM_COMMAND_GROUPS; ++i) {
        const auto& map = commands::groups[i];
        const auto it = map.find(std::string(name));

        if(it != map.cend()) {
            handler = it->second.first;
            required_permission = GROUP_PERMISSIONS[i];
            break;
        }
    }

    if(handler == nullptr) {
        sessions::send_notification(sender, Notification::T_TEXT_MESG, std::format("{}: unknown command", name));
        return;
    }

    auto permission = userlist::lookup(sender->public_key);

    if(permission < required_permission) {
        sessions::send_notification(sender, Notification::T_TEXT_MESG, std::format("{}: insufficient permissions", name));
        return;
    }

    handler(sender, arguments);
}

void commands::exec(Session* sender, std::string_view command)
{
    assert(sender);
    assert(sender->aes_context);
    assert(command.size());

    std::vector<std::string_view> parts;

    if(!strtools::tokenize(command, parts)) {
        sessions::send_notification(sender, Notification::T_TEXT_MESG, std::format("syntax error: {}", command));
        return;
    }

    if(parts.empty()) {
        sessions::send_notification(sender, Notification::T_TEXT_MESG, std::format("syntax error: {}", command));
        return;
    }

    std::string command_name(parts.front());
    parts.erase(parts.cbegin());

    try {
        exec(sender, command_name, parts);
    }
    catch(const std::exception& ex) {
        sessions::send_notification(sender, Notification::T_TEXT_MESG, std::format("error executing {}: {}", command_name, ex.what()));
    }
    catch(...) {
        sessions::send_notification(sender, Notification::T_TEXT_MESG, std::format("unknown error executing {}", command_name));
    }
}
