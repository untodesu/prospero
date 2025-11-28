// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: commands_admin.cc; Created: Thu Nov 27 2025 14:10:54
// Description: CommandGroup::Admin commands

#include "server/precompiled.hh"

#include "server/commands_admin.hh"

#include "core/exception.hh"
#include "core/protocol.hh"

#include "server/commands.hh"
#include "server/sessions.hh"
#include "server/userlist.hh"

static void cmd_adduser(Session* sender, const std::vector<std::string_view>& arguments)
{
    assert(sender);
    assert(sender->aes_context);

    if(arguments.empty()) {
        throw core::invalid_argument("missing argument");
    }

    ed25519::pkey_buffer public_key;

    if(!ed25519::import_public_key(arguments[0], public_key)) {
        throw core::invalid_argument("invalid public key");
    }

    if(userlist::lookup(public_key)) {
        throw core::invalid_argument("user exists");
    }

    userlist::modify(public_key, PERM_USER);

    sessions::send_notification(sender, Notification::T_TEXT_MESG, "user list modified");
}

static void cmd_moduser(Session* sender, const std::vector<std::string_view>& arguments)
{
    assert(sender);
    assert(sender->aes_context);

    if(arguments.size() < 2) {
        throw core::invalid_argument("missing argument");
    }

    ed25519::pkey_buffer target_pkey;
    std::uint32_t target_permission;

    auto target = sessions::lookup(std::string(arguments[0]));

    if(target && target->aes_context) {
        target_pkey = target->public_key;
    }
    else if(!ed25519::import_public_key(arguments[0], target_pkey)) {
        throw core::invalid_argument("invalid public key or username");
    }

    if(0 == std::memcmp(sender->public_key.data(), target_pkey.data(), sizeof(ed25519::pkey_buffer))) {
        throw core::invalid_argument("cannot modify your own userlist entry");
    }

    target_permission = userlist::lookup(target_pkey);

    if(target_permission == PERM_NULL) {
        throw core::invalid_argument("user is not present");
    }

    auto my_permission = userlist::lookup(sender->public_key);

    if(target_permission >= my_permission) {
        throw core::invalid_argument("you have no power here");
    }

    auto new_permission = userlist::string_to_permission(arguments[1]);

    if(new_permission == PERM_NULL && arguments[1].compare("null")) {
        throw core::invalid_argument("invalid permission value");
    }

    if(new_permission >= my_permission) {
        throw core::invalid_argument("you have no power here");
    }

    userlist::modify(target_pkey, new_permission);

    sessions::send_notification(sender, Notification::T_TEXT_MESG, "user list modified");
}

static void cmd_kick(Session* sender, const std::vector<std::string_view>& arguments)
{
    assert(sender);
    assert(sender->aes_context);

    if(arguments.empty()) {
        throw core::invalid_argument("missing argument");
    }

    if(auto target = sessions::lookup(std::string(arguments[0]))) {
        auto target_permission = userlist::lookup(target->public_key);
        auto my_permission = userlist::lookup(sender->public_key);

        if(target_permission >= my_permission) {
            throw core::invalid_argument("you have no power here");
        }

        if(0 == std::memcmp(sender->public_key.data(), target->public_key.data(), sizeof(ed25519::pkey_buffer))) {
            throw core::invalid_argument("cannot kick yourself");
        }

        enet_peer_disconnect_later(target->peer, 0U);

        sessions::broadcast_notification(Notification::T_MODR_KICK, target->username);

        return;
    }

    throw core::invalid_argument("{}: user not found", arguments[0]);
}

void commands::admin::init(void)
{
    commands::add(CommandGroup::Admin, "adduser", &cmd_adduser, "<public_key>");
    commands::add(CommandGroup::Admin, "moduser", &cmd_moduser, "<username|public_key> <permission>");
    commands::add(CommandGroup::Admin, "kick", &cmd_kick, "<username>");
}
