// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: config.hh; Created: Mon Nov 24 2025 22:25:08
// Description: Configuration parser

#ifndef CORE_CONFIG_HH
#define CORE_CONFIG_HH 1
#pragma once

#include "core/exports.hh"

namespace core
{
class Config final : public std::unordered_map<std::string, std::string> {
public:
    using std::unordered_map<std::string, std::string>::unordered_map;

    Config(void) = default;
    CORE_API explicit Config(std::istream& stream);
    CORE_API explicit Config(const std::filesystem::path& path);
    virtual ~Config(void) = default;

    CORE_API void read(std::istream& stream, bool append = false);
    CORE_API bool read(const std::filesystem::path& path, bool append = false);

    CORE_API void write(std::ostream& stream) const;
    CORE_API bool write(const std::filesystem::path& path) const;

    template<typename T>
    CORE_API T value(std::string_view key, T default_value = T()) const;

    template<typename T>
    CORE_API void set_value(std::string_view key, T value);
};
} // namespace core

#endif
