// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: config.hh; Created: Mon Nov 24 2025 22:25:08
// Description: Configuration parser

#ifndef CORE_CONFIG_HH
#define CORE_CONFIG_HH 1
#pragma once

class Config final : public std::unordered_map<std::string, std::string> {
public:
    using std::unordered_map<std::string, std::string>::unordered_map;

    Config(void) = default;
    explicit Config(std::istream& stream);
    explicit Config(const std::filesystem::path& path);
    virtual ~Config(void) = default;

    void read(std::istream& stream, bool append = false);
    bool read(const std::filesystem::path& path, bool append = false);

    void write(std::ostream& stream) const;
    bool write(const std::filesystem::path& path) const;

    template<typename T>
    T value(std::string_view key, T default_value = T()) const;

    template<typename T>
    void set_value(std::string_view key, T value);
};

#endif
