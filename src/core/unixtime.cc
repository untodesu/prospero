// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: unixtime.cc; Created: Mon Nov 24 2025 22:23:13
// Description: Unix time utilities

#include "core/precompiled.hh"

#include "core/unixtime.hh"

CORE_API std::uint64_t core::unix_seconds(void)
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(duration);
    return static_cast<std::uint64_t>(elapsed.count());
}

CORE_API std::uint64_t core::unix_milliseconds(void)
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    return static_cast<std::uint64_t>(elapsed.count());
}

CORE_API std::uint64_t core::unix_microseconds(void)
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    return static_cast<std::uint64_t>(elapsed.count());
}

CORE_API std::int64_t core::signed_unix_seconds(void)
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(duration);
    return static_cast<std::int64_t>(elapsed.count());
}

CORE_API std::int64_t core::signed_unix_milliseconds(void)
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    return static_cast<std::int64_t>(elapsed.count());
}

CORE_API std::int64_t core::signed_unix_microseconds(void)
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    return static_cast<std::int64_t>(elapsed.count());
}
