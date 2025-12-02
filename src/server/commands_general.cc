// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: commands_general.cc; Created: Thu Nov 27 2025 14:09:58
// Description: CommandGroup::General commands

#include "server/precompiled.hh"

#include "server/commands_general.hh"

#include "core/exception.hh"
#include "core/protocol.hh"
#include "core/unixtime.hh"

#include "server/commands.hh"
#include "server/sessions.hh"
#include "server/userlist.hh"

static void cmd_help(Session* sender, const std::vector<std::string_view>& arguments)
{
    assert(sender);
    assert(sender->aes_context);

    if(arguments.empty()) {
        std::ostringstream stream;
        auto first_in_list = true;

        for(std::size_t i = 0U; i < NUM_COMMAND_GROUPS; ++i) {
            for(const auto& it : commands::groups[i]) {
                if(!first_in_list)
                    stream << std::endl;
                first_in_list = false;

                stream << std::format("- {}", it.first);
            }
        }

        sessions::send_notification(sender, Notification::T_TEXT_MESG, stream.str());
    }
    else {
        for(std::size_t i = 0U; i < NUM_COMMAND_GROUPS; ++i) {
            for(const auto& it : commands::groups[i]) {
                if(it.first == arguments[0]) {
                    const auto& usage = it.second.second;

                    std::string message;

                    if(usage.empty()) {
                        message = std::format("usage: {}", it.first);
                    }
                    else {
                        message = std::format("usage: {} {}", it.first, usage);
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

static void cmd_image(Session* sender, const std::vector<std::string_view>& arguments)
{
    assert(sender);
    assert(sender->aes_context);

    if(arguments.empty()) {
        throw core::invalid_argument("missing argument");
    }

    constexpr static std::array allowed_schemas = { "http://", "https://" };
    constexpr static std::array allowed_extensions = { ".png", ".jpg", ".jpeg", ".gif", ".bmp", ".webp" };

    const auto& source_url = arguments[0];

    auto invalid_schema = true;
    auto invalid_extension = true;

    for(auto& schema : allowed_schemas) {
        if(source_url.starts_with(schema)) {
            invalid_schema = false;
            break;
        }
    }

    if(invalid_schema) {
        throw core::invalid_argument("invalid schema");
    }

    // The URL may have some additional arguments past
    // the file extension so we find the first ever
    // argument delimiter and only check up to that point
    auto end_of_url = source_url.find_first_of(" ?&");

    std::string_view trimmed_url;

    if(end_of_url == std::string_view::npos) {
        trimmed_url = source_url;
    }
    else {
        trimmed_url = source_url.substr(0U, end_of_url);
    }

    for(auto& extension : allowed_extensions) {
        if(trimmed_url.ends_with(extension)) {
            invalid_extension = false;
            break;
        }
    }

    if(invalid_extension) {
        throw core::invalid_argument("invalid file extension");
    }

    thread_local ImageMessage packet;

    packet.timestamp = unixtime::milliseconds();
    packet.username = sender->username;
    packet.source = source_url;

    sessions::broadcast_packet(packet);
}

void commands::general::init(void)
{
    commands::add(CommandGroup::General, "help", &cmd_help, "[command]");
    commands::add(CommandGroup::General, "list", &cmd_list, "");
    commands::add(CommandGroup::General, "image", &cmd_image, "<url>");
}
