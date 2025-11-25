// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: strtools.hh; Created: Mon Nov 24 2025 22:28:14
// Description: String-view utilities

#ifndef CORE_STRTOOLS_HH
#define CORE_STRTOOLS_HH 1
#pragma once

namespace strtools
{
bool is_whitespace(std::string_view string);
bool has_whitespace(std::string_view string);
std::string_view skip_whitespace(std::string_view string);
std::string_view trim_whitespace(std::string_view string);
bool tokenize(std::string_view string, std::vector<std::string_view>& parts);
} // namespace strtools

#endif
