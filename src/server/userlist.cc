// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: userlist.cc; Created: Tue Nov 25 2025 00:39:32
// Description: User access management

#include "server/precompiled.hh"

#include "server/userlist.hh"

#include "core/config.hh"
#include "core/crc64ecma.hh"

std::unordered_map<std::uint64_t, std::pair<core::ed25519::pkey_buffer, std::uint32_t>> userlist::map;

static std::filesystem::path userlist_filepath;

static const char* permission_to_string(std::uint32_t permission)
{
    switch(permission) {
        case PERM_USER:
            return "user";

        case PERM_OPER:
            return "oper";
    }

    return nullptr;
}

static std::uint32_t string_to_permission(std::string_view string)
{
    if(0 == string.compare("user")) {
        return PERM_USER;
    }

    if(0 == string.compare("oper")) {
        return PERM_OPER;
    }

    return PERM_NULL;
}

static void sync_to_disk(void)
{
    core::Config config;

    for(const auto& it : userlist::map) {
        const auto& public_key = it.second.first;
        const auto& permission = it.second.second;

        config.set_value<std::string_view>(core::ed25519::export_public_key(public_key), permission_to_string(permission));
    }

    config.write(userlist_filepath);
}

void userlist::init(const std::filesystem::path& config_directory)
{
    std::filesystem::create_directories(config_directory);
    userlist_filepath = config_directory / "userlist.conf";

    core::Config config;

    LOG_DEBUG("loading userlist from {}", userlist_filepath.string());

    if(config.read(userlist_filepath)) {
        for(const auto& it : config) {
            core::ed25519::pkey_buffer public_key;
            std::uint32_t permission = string_to_permission(it.second);

            if(permission && core::ed25519::import_public_key(it.first, public_key)) {
                userlist::map.emplace(core::crc64ecma(public_key), std::make_pair(public_key, permission));
                continue;
            }

            LOG_WARNING("skipping invalid user entry: {}", it.first);
        }
    }

    sync_to_disk();
}

void userlist::modify(const core::ed25519::pkey_buffer& key, std::uint32_t permission)
{
    auto hash = core::crc64ecma(key);

    if(permission == PERM_NULL) {
        userlist::map.erase(hash);
    }
    else {
        userlist::map[hash] = std::make_pair(key, permission);
    }

    sync_to_disk();
}

std::uint32_t userlist::lookup(const core::ed25519::pkey_buffer& key)
{
    auto it = userlist::map.find(core::crc64ecma(key));

    if(it == userlist::map.end()) {
        return PERM_NULL;
    }

    return it->second.second;
}