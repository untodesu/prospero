// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: unixtime.hh; Created: Mon Nov 24 2025 22:22:13
// Description: Unix time utilities

#ifndef CORE_UNIXTIME_HH
#define CORE_UNIXTIME_HH 1
#pragma once

#include "core/exports.hh"

namespace core
{
CORE_API std::uint64_t unix_seconds(void);
CORE_API std::uint64_t unix_milliseconds(void);
CORE_API std::uint64_t unix_microseconds(void);
} // namespace core

namespace core
{
CORE_API std::int64_t signed_unix_seconds(void);
CORE_API std::int64_t signed_unix_milliseconds(void);
CORE_API std::int64_t signed_unix_microseconds(void);
} // namespace core

#endif
