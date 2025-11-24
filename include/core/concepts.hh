// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: concepts.hh; Created: Mon Nov 24 2025 22:08:31
// Description: Non-standard concepts used in the project

#ifndef CORE_CONCEPTS_HH
#define CORE_CONCEPTS_HH 1
#pragma once

namespace core
{
template<typename T>
concept arithmetic = std::is_arithmetic_v<T>;
template<typename T>
concept signed_arithmetic = std::is_arithmetic_v<T> && std::is_signed_v<T>;
template<typename T>
concept unsigned_arithmetic = std::is_arithmetic_v<T> && std::is_unsigned_v<T>;
} // namespace core

#endif // CORE_CONCEPTS_HH
