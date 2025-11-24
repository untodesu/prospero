// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: string.hh; Created: Mon Nov 24 2025 22:28:14
// Description: String-view utilities

#ifndef CORE_STRING_HH
#define CORE_STRING_HH 1
#pragma once

#include "core/exports.hh"

namespace core
{
CORE_API bool is_whitespace(std::string_view string);
CORE_API bool has_whitespace(std::string_view string);
CORE_API std::string_view skip_whitespace(std::string_view string);
CORE_API std::string_view trim_whitespace(std::string_view string);
CORE_API bool tokenize(std::string_view string, std::vector<std::string_view>& parts);
} // namespace core

#endif
