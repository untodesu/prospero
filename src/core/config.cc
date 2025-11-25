// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: config.cc; Created: Mon Nov 24 2025 22:27:36
// Description: Configuration parser

#include "core/precompiled.hh"

#include "core/config.hh"

#include "core/strtools.hh"

template<std::unsigned_integral T>
static T config_value_autobase(const Config* config, const std::string& key, T default_value)
{
    auto it = config->find(std::string(key));

    if(it == config->cend()) {
        return default_value;
    }

    auto deduced_base = 10;
    std::string_view value(it->second);

    if(value.starts_with("0x") || value.starts_with("0X")) {
        value = value.substr(2);
        deduced_base = 16;
    }
    else if(value.starts_with("0b") || value.starts_with("0B")) {
        value = value.substr(2);
        deduced_base = 2;
    }

    T result;
    auto check = std::from_chars(value.data(), value.data() + value.size(), result, deduced_base);

    if(check.ec == std::errc()) {
        return result;
    }

    return default_value;
}

template<typename T>
requires std::is_arithmetic_v<T>
static T config_value_arithmetic(const Config* config, const std::string& key, T default_value)
{
    auto it = config->find(std::string(key));

    if(it == config->cend()) {
        return default_value;
    }

    T result;
    auto check = std::from_chars(it->second.data(), it->second.data() + it->second.size(), result);

    if(check.ec == std::errc()) {
        return result;
    }

    return default_value;
}

Config::Config(std::istream& stream)
{
    read(stream);
}

Config::Config(const std::filesystem::path& path)
{
    read(path);
}

void Config::read(std::istream& stream, bool append)
{
    assert(stream.good());

    if(!append) {
        this->clear();
    }

    std::string line;
    std::string kv_string;

    while(std::getline(stream, line)) {
        auto comment = line.find_first_of('#');

        if(comment == std::string::npos) {
            kv_string = strtools::trim_whitespace(line);
        }
        else {
            kv_string = strtools::trim_whitespace(line.substr(0, comment));
        }

        if(strtools::is_whitespace(kv_string)) {
            // Ignore empty or commented out lines
            continue;
        }

        auto separator = kv_string.find_first_of('=');

        if(separator == std::string_view::npos) {
            continue;
        }

        auto kv_name = kv_string.substr(0, separator);
        auto kv_value = kv_string.substr(separator + 1);

        kv_name = strtools::trim_whitespace(kv_name);
        kv_value = strtools::trim_whitespace(kv_value);

        this->insert_or_assign(std::string(kv_name), std::string(kv_value));
    }
}

bool Config::read(const std::filesystem::path& path, bool append)
{
    std::ifstream stream(path, std::ios::in);

    if(stream.is_open()) {
        read(stream, append);
        return true;
    }

    return false;
}

void Config::write(std::ostream& stream) const
{
    assert(stream.good());

    auto curtime = std::time(nullptr);

    stream << "# Prospero Configuration File" << std::endl;
    stream << "# Generated at: " << std::put_time(std::gmtime(&curtime), "%Y-%m-%d %H:%M:%S %z") << std::endl << std::endl;

    for(const auto& it : *this) {
        stream << it.first << " = ";
        stream << it.second;
        stream << std::endl;
    }
}

bool Config::write(const std::filesystem::path& path) const
{
    std::ofstream stream(path, std::ios::out | std::ios::trunc);

    if(stream.is_open()) {
        write(stream);
        return true;
    }

    return false;
}

template<>
unsigned char Config::value<unsigned char>(std::string_view key, unsigned char default_value) const
{
    return config_value_autobase(this, std::string(key), default_value);
}

template<>
unsigned short Config::value<unsigned short>(std::string_view key, unsigned short default_value) const
{
    return config_value_autobase(this, std::string(key), default_value);
}

template<>
unsigned int Config::value<unsigned int>(std::string_view key, unsigned int default_value) const
{
    return config_value_autobase(this, std::string(key), default_value);
}

template<>
unsigned long Config::value<unsigned long>(std::string_view key, unsigned long default_value) const
{
    return config_value_autobase(this, std::string(key), default_value);
}

template<>
unsigned long long Config::value<unsigned long long>(std::string_view key, unsigned long long default_value) const
{
    return config_value_autobase(this, std::string(key), default_value);
}

template<>
char Config::value<char>(std::string_view key, char default_value) const
{
    return static_cast<char>(config_value_arithmetic(this, std::string(key), static_cast<int>(default_value)));
}

template<>
short Config::value<short>(std::string_view key, short default_value) const
{
    return config_value_arithmetic(this, std::string(key), default_value);
}

template<>
int Config::value<int>(std::string_view key, int default_value) const
{
    return config_value_arithmetic(this, std::string(key), default_value);
}

template<>
long Config::value<long>(std::string_view key, long default_value) const
{
    return config_value_arithmetic(this, std::string(key), default_value);
}

template<>
long long Config::value<long long>(std::string_view key, long long default_value) const
{
    return config_value_arithmetic(this, std::string(key), default_value);
}

template<>
float Config::value<float>(std::string_view key, float default_value) const
{
    return config_value_arithmetic(this, std::string(key), default_value);
}

template<>
double Config::value<double>(std::string_view key, double default_value) const
{
    return config_value_arithmetic(this, std::string(key), default_value);
}

template<>
std::string_view Config::value<std::string_view>(std::string_view key, std::string_view default_value) const
{
    auto it = this->find(std::string(key));

    if(it == this->cend()) {
        return default_value;
    }

    return it->second;
}

template<>
void Config::set_value<unsigned char>(std::string_view key, unsigned char value)
{
    this->insert_or_assign(std::string(key), std::to_string(value));
}

template<>
void Config::set_value<unsigned short>(std::string_view key, unsigned short value)
{
    this->insert_or_assign(std::string(key), std::to_string(value));
}

template<>
void Config::set_value<unsigned int>(std::string_view key, unsigned int value)
{
    this->insert_or_assign(std::string(key), std::to_string(value));
}

template<>
void Config::set_value<unsigned long>(std::string_view key, unsigned long value)
{
    this->insert_or_assign(std::string(key), std::to_string(value));
}

template<>
void Config::set_value<unsigned long long>(std::string_view key, unsigned long long value)
{
    this->insert_or_assign(std::string(key), std::to_string(value));
}

template<>
void Config::set_value<char>(std::string_view key, char value)
{
    this->insert_or_assign(std::string(key), std::to_string(static_cast<int>(value)));
}

template<>
void Config::set_value<short>(std::string_view key, short value)
{
    this->insert_or_assign(std::string(key), std::to_string(value));
}

template<>
void Config::set_value<int>(std::string_view key, int value)
{
    this->insert_or_assign(std::string(key), std::to_string(value));
}

template<>
void Config::set_value<long>(std::string_view key, long value)
{
    this->insert_or_assign(std::string(key), std::to_string(value));
}

template<>
void Config::set_value<long long>(std::string_view key, long long value)
{
    this->insert_or_assign(std::string(key), std::to_string(value));
}

template<>
void Config::set_value<float>(std::string_view key, float value)
{
    this->insert_or_assign(std::string(key), std::to_string(value));
}

template<>
void Config::set_value<double>(std::string_view key, double value)
{
    this->insert_or_assign(std::string(key), std::to_string(value));
}

template<>
void Config::set_value<std::string_view>(std::string_view key, std::string_view value)
{
    this->insert_or_assign(std::string(key), std::string(value));
}
