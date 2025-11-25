// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: ed25519.hh; Created: Mon Nov 24 2025 22:32:01
// Description: libed25519 wrapper so only core.dll depends on it

#ifndef CORE_ED25519_HH
#define CORE_ED25519_HH 1
#pragma once

namespace ed25519
{
using seed_buffer = std::array<std::byte, 32>;
using pkey_buffer = std::array<std::byte, 32>;
using skey_buffer = std::array<std::byte, 64>;
using exch_buffer = std::array<std::byte, 32>; ///< shared secret
using sign_buffer = std::array<std::byte, 64>;
} // namespace ed25519

namespace ed25519
{
void generate_seed(seed_buffer& seed);
void generate_keys(const seed_buffer& seed, pkey_buffer& pkey, skey_buffer& skey);
void generate_exch(const skey_buffer& skey, const pkey_buffer& pkey, exch_buffer& exch);
} // namespace ed25519

namespace ed25519
{
void sign(const pkey_buffer& pkey, const skey_buffer& skey, std::span<const std::byte> message, sign_buffer& sign);
bool verify(const pkey_buffer& pkey, std::span<const std::byte> message, const sign_buffer& sign);
} // namespace ed25519

namespace ed25519
{
bool import_public_key(std::string_view hexstring, pkey_buffer& pkey);
bool import_private_key(std::string_view hexstring, skey_buffer& skey);
std::string export_public_key(const pkey_buffer& pkey);
std::string export_private_key(const skey_buffer& skey);
} // namespace ed25519

#endif
