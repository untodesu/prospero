// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: aes256.hh; Created: Mon Nov 24 2025 22:51:26
// Description: libaes wrapper so only core.dll depends on it

#ifndef CORE_AES256_HH
#define CORE_AES256_HH 1
#pragma once

#include "core/exports.hh"

namespace core::aes256
{
constexpr static std::size_t BLOCK_SIZE = 16U; ///< Buffers must be multiple of this size
} // namespace core::aes256

namespace core::aes256
{
using context = void*;
using skey_buffer = std::array<std::byte, 32>;
using ivec_buffer = std::array<std::byte, 16>;
} // namespace core::aes256

namespace core::aes256
{
CORE_API void create(context& context, const skey_buffer& key);
CORE_API void destroy(context& context);
} // namespace core::aes256

namespace core::aes256
{
CORE_API void set_ivec(context& context, const ivec_buffer& ivec);
CORE_API void encrypt_insitu(context& context, std::span<std::byte> data);
CORE_API void decrypt_insitu(context& context, std::span<std::byte> data);
} // namespace core::aes256

#endif // CORE_AES256_HH
