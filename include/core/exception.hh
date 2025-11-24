// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: exception.hh; Created: Mon Nov 24 2025 22:10:23
// Description: Exceptions with formatted messages

#ifndef CORE_EXCEPTION_HH
#define CORE_EXCEPTION_HH 1
#pragma once

namespace core::detail
{
template<typename T>
concept derived_exception = std::derived_from<T, std::logic_error> || std::derived_from<T, std::runtime_error>;
} // namespace core::detail

namespace core
{
template<detail::derived_exception T>
class formatted_exception final : public T {
public:
    template<typename... AT>
    explicit formatted_exception(const std::format_string<AT...>& fmt, AT&&... args);
    virtual ~formatted_exception(void) override = default;
};
} // namespace core

namespace core
{
using logic_error = formatted_exception<std::logic_error>;
using invalid_argument = formatted_exception<std::invalid_argument>;
using domain_error = formatted_exception<std::domain_error>;
using length_error = formatted_exception<std::length_error>;
} // namespace core

namespace core
{
using runtime_error = formatted_exception<std::runtime_error>;
using range_error = formatted_exception<std::range_error>;
using overflow_error = formatted_exception<std::overflow_error>;
using underflow_error = formatted_exception<std::underflow_error>;
} // namespace core

template<core::detail::derived_exception T>
template<typename... AT>
core::formatted_exception<T>::formatted_exception(const std::format_string<AT...>& fmt, AT&&... args)
    : T(std::vformat(fmt.get(), std::make_format_args(args...)))
{
    // empty
}

#endif // CORE_EXCEPTION_HH
