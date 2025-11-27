// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: userlist.hh; Created: Tue Nov 25 2025 00:37:47
// Description: User access management

#ifndef SERVER_USERLIST_HH
#define SERVER_USERLIST_HH 1
#pragma once

#include "core/ed25519.hh"

constexpr static std::uint32_t PERM_NULL = 0U; ///< No server access
constexpr static std::uint32_t PERM_USER = 1U; ///< Regular user access
constexpr static std::uint32_t PERM_OPER = 2U; ///< Operator access
constexpr static std::uint32_t PERM_ROOT = 3U; ///< Root aka full access

namespace userlist
{
extern std::unordered_map<std::uint64_t, std::pair<ed25519::pkey_buffer, std::uint32_t>> map;
} // namespace userlist

namespace userlist
{
const char* permission_to_string(std::uint32_t permission);
std::uint32_t string_to_permission(std::string_view string);
} // namespace userlist

namespace userlist
{
void init(const std::filesystem::path& config_directory);
void modify(const ed25519::pkey_buffer& public_key, std::uint32_t permission);
std::uint32_t lookup(const ed25519::pkey_buffer& public_key);
} // namespace userlist

#endif
