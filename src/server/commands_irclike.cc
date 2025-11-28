// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: commands_irclike.cc; Created: Sat Nov 29 2025 02:22:38
// Description: CommandGroup::IrcLike commands

#include "server/precompiled.hh"

#include "server/commands_irclike.hh"

#include "core/exception.hh"
#include "core/protocol.hh"

#include "server/commands.hh"
#include "server/sessions.hh"
#include "server/userlist.hh"

static void cmd_away(Session* sender, const std::vector<std::string_view>& arguments)
{
    assert(sender);
    assert(sender->aes_context);

    if(arguments.empty()) {
        if(sender->away_message.empty()) {
            throw core::invalid_argument("missing argument");
        }

        sender->away_message.clear();
        sessions::broadcast_notification(Notification::T_USER_BACK, sender->username);
        return;
    }

    std::ostringstream stream;

    for(std::size_t i = 0U; i < arguments.size(); ++i) {
        if(i > 0U)
            stream << ' ';
        stream << arguments[i];
    }

    sender->away_message = stream.str();

    sessions::broadcast_notification(Notification::T_USER_AWAY, sender->username, sender->away_message);
}

static void cmd_me(Session* sender, const std::vector<std::string_view>& arguments)
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

static void cmd_motd(Session* sender, const std::vector<std::string_view>& arguments)
{
    throw core::invalid_argument("not implemented but planned");
}

static void cmd_whois(Session* sender, const std::vector<std::string_view>& arguments)
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

void commands::irclike::init(void)
{
    commands::add(CommandGroup::IrcLike, "away", &cmd_away, "[message]");
    commands::add(CommandGroup::IrcLike, "me", &cmd_me, "<action>");
    commands::add(CommandGroup::IrcLike, "motd", &cmd_motd, "");
    commands::add(CommandGroup::IrcLike, "whois", &cmd_whois, "<username>");
}
