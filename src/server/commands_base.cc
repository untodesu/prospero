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

static void command_list(Session* sender, const std::vector<std::string_view>& arguments)
{
    assert(sender);
    assert(sender->aes_context);

    std::ostringstream stream;

    for(std::size_t i = 0U; i < sessions::vector.size(); ++i) {
        if(sessions::vector[i].peer && sessions::vector[i].aes_context) {
            if(i >= 1U)
                stream << std::endl;
            stream << std::format("- {}", sessions::vector[i].username);
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

    skeleton.handler = &command_list;
    commands::add("list", skeleton);

    skeleton.handler = &command_rpme;
    commands::add("me", skeleton);

    skeleton.handler = &command_whois;
    commands::add("whois", skeleton);
}
