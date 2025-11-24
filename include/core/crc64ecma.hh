// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: crc64ecma.hh; Created: Mon Nov 24 2025 22:14:23
// Description: ECMA-182 CRC-64 implementation

#ifndef CORE_CRC64ECMA_HH
#define CORE_CRC64ECMA_HH 1
#pragma once

#include "core/exports.hh"

namespace core
{
CORE_API std::uint64_t crc64ecma(const void* buffer, std::size_t size, std::uint64_t previous = 0U);
CORE_API std::uint64_t crc64ecma(std::span<const std::byte> buffer, std::uint64_t previous = 0U);
CORE_API std::uint64_t crc64ecma(std::span<const char> buffer, std::uint64_t previous = 0U);
} // namespace core

#endif // CORE_CRC64ECMA_HH
