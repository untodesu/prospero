// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: ed25519.hh; Created: Mon Nov 24 2025 22:32:01
// Description: libed25519 wrapper so only core.dll depends on it

#ifndef CORE_ED25519_HH
#define CORE_ED25519_HH 1
#pragma once

#include "core/exports.hh"

namespace core::ed25519
{
using seed_buffer = std::array<std::byte, 32>;
using pkey_buffer = std::array<std::byte, 32>;
using skey_buffer = std::array<std::byte, 64>;
using exch_buffer = std::array<std::byte, 32>; ///< shared secret
using sign_buffer = std::array<std::byte, 64>;
} // namespace core::ed25519

namespace core::ed25519
{
CORE_API void generate_seed(seed_buffer& seed);
CORE_API void generate_keys(const seed_buffer& seed, pkey_buffer& pkey, skey_buffer& skey);
CORE_API void generate_exch(const skey_buffer& skey, const pkey_buffer& pkey, exch_buffer& exch);
} // namespace core::ed25519

namespace core::ed25519
{
CORE_API void sign(const pkey_buffer& pkey, const skey_buffer& skey, std::span<const std::byte> message, sign_buffer& sign);
CORE_API bool verify(const pkey_buffer& pkey, std::span<const std::byte> message, const sign_buffer& sign);
} // namespace core::ed25519

namespace core::ed25519
{
CORE_API bool import_public_key(std::string_view hexstring, pkey_buffer& pkey);
CORE_API bool import_private_key(std::string_view hexstring, skey_buffer& skey);
CORE_API std::string export_public_key(const pkey_buffer& pkey);
CORE_API std::string export_private_key(const skey_buffer& skey);
} // namespace core::ed25519

#endif // CORE_ED25519_HH
