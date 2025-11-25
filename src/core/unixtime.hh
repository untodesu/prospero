// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: unixtime.hh; Created: Mon Nov 24 2025 22:22:13
// Description: Unix time utilities

#ifndef CORE_UNIXTIME_HH
#define CORE_UNIXTIME_HH 1
#pragma once

namespace unixtime
{
std::uint64_t seconds(void);
std::uint64_t milliseconds(void);
std::uint64_t microseconds(void);
} // namespace unixtime

namespace unixtime
{
std::int64_t signed_seconds(void);
std::int64_t signed_milliseconds(void);
std::int64_t signed_microseconds(void);
} // namespace unixtime

#endif
