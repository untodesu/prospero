// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: aes256.hh; Created: Mon Nov 24 2025 22:51:26
// Description: libaes wrapper so only core.dll depends on it

#ifndef CORE_AES256_HH
#define CORE_AES256_HH 1
#pragma once

namespace aes256
{
constexpr static std::size_t BLOCK_SIZE = 16U; ///< Buffers must be multiple of this size
} // namespace aes256

namespace aes256
{
using context = void*;
using skey_buffer = std::array<std::byte, 32>;
using ivec_buffer = std::array<std::byte, 16>;
} // namespace aes256

namespace aes256
{
void create(context& context, const skey_buffer& key);
void destroy(context& context);
} // namespace aes256

namespace aes256
{
void set_ivec(context& context, const ivec_buffer& ivec);
void encrypt_insitu(context& context, std::span<std::byte> data);
void decrypt_insitu(context& context, std::span<std::byte> data);
} // namespace aes256

#endif
