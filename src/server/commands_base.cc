// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: commands_base.cc; Created: Thu Nov 27 2025 14:09:58
// Description: Base command set available to all users

#include "server/precompiled.hh"

#include "server/commands_base.hh"

#include "core/exception.hh"
#include "core/protocol.hh"

#include "server/commands.hh"
#include "server/sessions.hh"
#include "server/userlist.hh"

static void command_help(Session* sender, const std::vector<std::string_view>& arguments)
{
    assert(sender);
    assert(sender->aes_context);

    auto my_permission = userlist::lookup(sender->public_key);

    if(arguments.empty()) {
        std::ostringstream stream;
        stream << "Available commands:";

        for(auto it : commands::map) {
            if(my_permission >= it.second.permission) {
                stream << std::endl;
                stream << std::format("- {} {}", it.first, it.second.instructions);
            }
        }

        sessions::send_notification(sender, Notification::T_TEXT_MESG, stream.str());
    }
    else {
        auto it = commands::map.find(std::string(arguments[0]));

        if(it == commands::map.cend()) {
            throw core::invalid_argument("{}: unknown command", arguments[0]);
        }

        if(my_permission < it->second.permission) {
            throw core::invalid_argument("{}: insufficient permissions", arguments[0]);
        }

        sessions::send_notification(sender, Notification::T_TEXT_MESG, std::format("usage: {} {}", it->first, it->second.instructions));
    }
}

static void command_list(Session* sender, const std::vector<std::string_view>& arguments)
{
    assert(sender);
    assert(sender->aes_context);

    std::ostringstream stream;
    stream << "Online users:";

    for(auto& session : sessions::vector) {
        if(!session.username.empty()) {
            stream << std::endl;
            stream << std::format("- {}", session.username);
        }
    }

    sessions::send_notification(sender, Notification::T_TEXT_MESG, stream.str());
}

static void command_rpme(Session* sender, const std::vector<std::string_view>& arguments)
{
    assert(sender);
    assert(sender->aes_context);

    if(arguments.empty()) {
        throw core::invalid_argument("missing argument");
    }

    std::ostringstream stream;

    for(std::size_t i = 0U; i < arguments.size(); ++i) {
        if(i > 0U)
            stream << ' ';
        stream << arguments[i];
    }

    sessions::broadcast_notification(Notification::T_TEXT_MESG, std::format("* {} {}", sender->username, stream.str()));
}

static void command_whois(Session* sender, const std::vector<std::string_view>& arguments)
{
    assert(sender);
    assert(sender->aes_context);

    if(arguments.empty()) {
        throw core::invalid_argument("missing argument");
    }

    auto target = sessions::lookup(std::string(arguments[0]));

    if(target == nullptr || target->aes_context == nullptr) {
        throw core::invalid_argument("{}: user not found", arguments[0]);
    }

    auto permission = userlist::lookup(target->public_key);
    auto permission_string = userlist::permission_to_string(permission);

    if(permission_string == nullptr) {
        permission_string = "unknown";
    }

    auto public_key = ed25519::export_public_key(target->public_key);

    sessions::send_notification(sender, Notification::T_TEXT_MESG, std::format("{}: {} {}", target->username, permission_string, public_key));
}

void commands::base::init(void)
{
    Command skeleton;
    skeleton.permission = PERM_USER;

    skeleton.instructions = "[command]";
    skeleton.handler = &command_help;
    commands::add("help", skeleton);

    skeleton.instructions.clear();
    skeleton.handler = &command_list;
    commands::add("list", skeleton);

    skeleton.instructions = "<message>";
    skeleton.handler = &command_rpme;
    commands::add("me", skeleton);

    skeleton.instructions = "<username>";
    skeleton.handler = &command_whois;
    commands::add("whois", skeleton);
}
