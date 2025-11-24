// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: version.hh; Created: Mon Nov 24 2025 22:19:23
// Description: Project version information

#ifndef CORE_VERSION_HH
#define CORE_VERSION_HH 1
#pragma once

#include "core/exports.hh"

namespace core
{
CORE_API extern const unsigned int version_major;
CORE_API extern const unsigned int version_minor;
CORE_API extern const unsigned int version_patch;
} // namespace core

namespace core
{
CORE_API extern const std::string_view version_branch;
CORE_API extern const std::string_view version_commit;
CORE_API extern const std::string_view version_semver;
} // namespace core

#endif // CORE_VERSION_HH
