// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: unixtime.cc; Created: Mon Nov 24 2025 22:23:13
// Description: Unix time utilities

#include "core/precompiled.hh"

#include "core/unixtime.hh"

std::uint64_t unixtime::seconds(void)
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(duration);
    return static_cast<std::uint64_t>(elapsed.count());
}

std::uint64_t unixtime::milliseconds(void)
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    return static_cast<std::uint64_t>(elapsed.count());
}

std::uint64_t unixtime::microseconds(void)
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    return static_cast<std::uint64_t>(elapsed.count());
}

std::int64_t unixtime::signed_seconds(void)
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(duration);
    return static_cast<std::int64_t>(elapsed.count());
}

std::int64_t unixtime::signed_milliseconds(void)
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    return static_cast<std::int64_t>(elapsed.count());
}

std::int64_t unixtime::signed_microseconds(void)
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    return static_cast<std::int64_t>(elapsed.count());
}
