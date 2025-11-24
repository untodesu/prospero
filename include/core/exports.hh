// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: exports.hh; Created: Mon Nov 24 2025 22:06:52
// Description: Export and import macros for shared libraries

#ifndef CORE_EXPORTS_HH
#define CORE_EXPORTS_HH 1
#pragma once

#include "core/compiler.hh"

#if defined(PROSPERO_STATIC)
#define CORE_API
#elif defined(CORE_DLLPRIVATE)
#define CORE_API PR_DLLEXPORT
#else
#define CORE_API PR_DLLIMPORT
#endif

#endif // CORE_EXPORTS_HH
