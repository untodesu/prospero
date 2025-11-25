// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: identity.cc; Created: Mon Nov 24 2025 23:33:10
// Description: Server ed25519 keypair

#include "server/precompiled.hh"

#include "server/identity.hh"

#include "core/config.hh"

ed25519::pkey_buffer identity::public_key;
ed25519::skey_buffer identity::private_key;

void identity::init(const std::filesystem::path& config_directory)
{
    std::filesystem::create_directories(config_directory);
    std::filesystem::path filepath(config_directory / "identity.conf");

    LOG_DEBUG("loading server identity from {}", filepath.string());

    Config config(filepath);
    auto pkey_str = config.value<std::string_view>("pkey");
    auto skey_str = config.value<std::string_view>("skey");

    auto invalid = false;
    invalid = invalid || !ed25519::import_public_key(pkey_str, public_key);
    invalid = invalid || !ed25519::import_private_key(skey_str, private_key);

    if(invalid) {
        LOG_WARNING("identity file is missing or corrupted");
        LOG_WARNING("generating a new server identity keypair");

        ed25519::seed_buffer seed;
        ed25519::generate_seed(seed);
        ed25519::generate_keys(seed, public_key, private_key);

        config.set_value<std::string_view>("pkey", ed25519::export_public_key(public_key));
        config.set_value<std::string_view>("skey", ed25519::export_private_key(private_key));
        config.write(filepath);
    }
}
