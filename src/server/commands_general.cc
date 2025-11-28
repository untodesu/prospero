// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: commands_general.cc; Created: Thu Nov 27 2025 14:09:58
// Description: CommandGroup::General commands

#include "server/precompiled.hh"

#include "server/commands_general.hh"

#include "core/exception.hh"
#include "core/protocol.hh"

#include "server/commands.hh"
#include "server/sessions.hh"
#include "server/userlist.hh"

static void cmd_help(Session* sender, const std::vector<std::string_view>& arguments)
{
    assert(sender);
    assert(sender->aes_context);

    constexpr static std::array<std::string_view, NUM_COMMAND_GROUPS> group_names = { "general", "irc-like", "admin", "root" };

    if(arguments.empty()) {
        std::ostringstream stream;

        for(std::size_t i = 0U; i < NUM_COMMAND_GROUPS; ++i) {
            auto first_in_group = true;

            for(const auto& it : commands::groups[i]) {
                if(!first_in_group) {
                    first_in_group = false;
                    stream << std::endl;
                }

                stream << std::format("- [{}] {}", group_names[i], it.first);
            }
        }

        sessions::send_notification(sender, Notification::T_TEXT_MESG, stream.str());
    }
    else {
        for(std::size_t i = 0U; i < NUM_COMMAND_GROUPS; ++i) {
            for(const auto& it : commands::groups[i]) {
                if(it.first == arguments[0]) {
                    const auto& usage = it.second.second;
                    const auto& group_name = group_names[i];
                    std::string message;

                    if(usage.empty()) {
                        message = std::format("[{}] usage: {}", group_name, it.first);
                    }
                    else {
                        message = std::format("[{}] usage: {} {}", group_name, it.first, usage);
                    }

                    sessions::send_notification(sender, Notification::T_TEXT_MESG, message);

                    return;
                }
            }
        }

        throw core::invalid_argument("{}: unknown command", arguments[0]);
    }
}

static void cmd_list(Session* sender, const std::vector<std::string_view>& arguments)
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

void commands::general::init(void)
{
    commands::add(CommandGroup::General, "help", &cmd_help, "[command]");
    commands::add(CommandGroup::General, "list", &cmd_list, "");
}
