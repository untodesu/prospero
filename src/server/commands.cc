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

std::unordered_map<std::string, Command> commands::map;

void commands::add(std::string_view name, const Command& command)
{
    assert(name.size());
    assert(command.permission);
    assert(command.handler);

    map.insert_or_assign(std::string(name), command);
}

void commands::exec(Session* sender, std::string_view name, const std::vector<std::string_view>& arguments)
{
    assert(sender);
    assert(sender->aes_context);
    assert(name.size());

    auto it = map.find(std::string(name));

    if(it == map.cend()) {
        sessions::send_notification(sender, Notification::T_TEXT_MESG, std::format("{}: unknown command", name));
        return;
    }

    auto permission = userlist::lookup(sender->public_key);

    if(permission < it->second.permission) {
        sessions::send_notification(sender, Notification::T_TEXT_MESG, std::format("{}: insufficient permissions", name));
        return;
    }

    it->second.handler(sender, arguments);
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
