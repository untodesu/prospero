// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: main.cc; Created: Tue Nov 25 2025 00:45:41
// Description: Server main entry point

#include "server/precompiled.hh"

#include "core/exception.hh"
#include "core/version.hh"

#include "server/host.hh"
#include "server/identity.hh"
#include "server/sessions.hh"
#include "server/settings.hh"
#include "server/userlist.hh"

static std::atomic_bool is_running = false;

static void signal_handler(int signal_num)
{
    LOG_WARNING("received signal {}", signal_num);

    is_running = false;
}

int main(int argc, char** argv)
{
    uulog::add_sink(&uulog::builtin::stderr_ansi);

    LOG_INFO("prospero server {}", version::semver);

    try {
        auto enet_check = enet_initialize();

        if(enet_check) {
            throw core::runtime_error("enet_initialize failed");
        }

        std::filesystem::path config_directory("assets/etc/prospero"); // TODO: make configurable
        std::filesystem::create_directories(config_directory);

        settings::init(config_directory);

        identity::init(config_directory);
        userlist::init(config_directory);

        sessions::init();

        host::init();

        std::signal(SIGINT, &signal_handler);
        std::signal(SIGTERM, &signal_handler);

        is_running = true;

        while(is_running) {
            host::update();
        }

        host::shutdown();

        sessions::shutdown();

        settings::shutdown();

        enet_deinitialize();

        return EXIT_SUCCESS;
    }
    catch(const std::exception& ex) {
        LOG_CRITICAL("{}: {}", argv[0], ex.what());
        return EXIT_FAILURE;
    }
    catch(...) {
        LOG_CRITICAL("{}: unknown fatal error occurred", argv[0]);
        return EXIT_FAILURE;
    }
}
