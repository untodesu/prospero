// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: settings.cc; Created: Mon Nov 24 2025 23:25:57
// Description: Server settings

#include "server/precompiled.hh"

#include "server/settings.hh"

#include "core/config.hh"
#include "core/exception.hh"

static std::filesystem::path config_filepath;

unsigned short settings::host::udp_port = 18137U;
unsigned int settings::host::max_peers = 32U;
unsigned long settings::auth::timeout_ms = 10000U;

static void apply_from_config(const core::Config& config)
{
    settings::host::udp_port = config.value<unsigned short>("host.udp_port", settings::host::udp_port);
    settings::host::max_peers = config.value<unsigned int>("host.max_peers", settings::host::max_peers);
    settings::auth::timeout_ms = config.value<unsigned long>("auth.timeout_ms", settings::auth::timeout_ms);
}

static void apply_to_config(core::Config& config)
{
    config.set_value<unsigned short>("host.udp_port", settings::host::udp_port);
    config.set_value<unsigned int>("host.max_peers", settings::host::max_peers);
    config.set_value<unsigned long>("auth.timeout_ms", settings::auth::timeout_ms);
}

void settings::init(const std::filesystem::path& config_directory)
{
    std::filesystem::create_directories(config_directory);

    config_filepath = config_directory / "server.conf";

    core::Config config;

    if(config.read(config_filepath)) {
        LOG_DEBUG("reading configuration from {}", config_filepath.string());

        apply_from_config(config);
    }
    else {
        LOG_WARNING("no configuration found, using defaults");

        apply_to_config(config);

        config.write(config_filepath);
    }
}

void settings::shutdown(void)
{
    core::Config config;

    apply_to_config(config);

    config.write(config_filepath);
}
