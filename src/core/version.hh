// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: version.hh; Created: Mon Nov 24 2025 22:19:23
// Description: Version information

#ifndef CORE_VERSION_HH
#define CORE_VERSION_HH 1
#pragma once

namespace version
{
extern const unsigned int major;
extern const unsigned int minor;
extern const unsigned int patch;
} // namespace version

namespace version
{
extern const std::string_view branch;
extern const std::string_view commit;
extern const std::string_view semver;
} // namespace version

#endif
