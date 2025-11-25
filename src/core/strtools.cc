// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: strtools.cc; Created: Mon Nov 24 2025 22:29:36
// Description: String-view utilities

#include "core/precompiled.hh"

#include "core/strtools.hh"

constexpr static const char* WHITESPACE_CHARS = " \t\r\n";
constexpr static const char* TOKEN_SEPARATORS = " \t\r\n\"";
constexpr static char DOUBLE_QUOTE = '\"';

bool strtools::is_whitespace(std::string_view string)
{
    auto result = false;
    result = result || string.empty();
    result = result || string.size() == 1 && string[0] == 0x00;
    result = result || string.find_first_not_of(WHITESPACE_CHARS) == std::string_view::npos;
    return result;
}

bool strtools::has_whitespace(std::string_view string)
{
    return string.find_first_of(WHITESPACE_CHARS) != std::string_view::npos;
}

std::string_view strtools::skip_whitespace(std::string_view string)
{
    std::size_t position = 0;

    while(position < string.size() && std::isspace(string[position])) {
        position += 1;
    }

    return std::string_view(string.data() + position, string.size() - position);
}

std::string_view strtools::trim_whitespace(std::string_view string)
{
    auto start = string.find_first_not_of(WHITESPACE_CHARS);
    auto end = string.find_last_not_of(WHITESPACE_CHARS);

    if(start == std::string_view::npos || end == std::string_view::npos) {
        return {};
    }

    return string.substr(start, end - start + 1);
}

bool strtools::tokenize(std::string_view string, std::vector<std::string_view>& parts)
{
    parts.clear();

    while(string.size()) {
        string = strtools::skip_whitespace(string);

        if(string.empty()) {
            break;
        }

        if(string.front() == DOUBLE_QUOTE) {
            auto pos = string.find(DOUBLE_QUOTE, 1);

            if(pos == std::string_view::npos) {
                // Unmatched quotation
                return false;
            }

            parts.emplace_back(string.substr(1, pos - 1));
            string = string.substr(pos + 1);
            continue;
        }

        auto pos = string.find_first_of(TOKEN_SEPARATORS);

        if(pos == std::string_view::npos) {
            parts.emplace_back(string);
            break;
        }

        if(pos > 0) {
            parts.emplace_back(string.substr(0, pos));
            string.remove_prefix(pos);
        }
    }

    return true;
}
