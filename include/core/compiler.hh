// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: compiler.hh; Created: Mon Nov 24 2025 22:05:28
// Description: Compiler-specific definitions and macros

#ifndef CORE_COMPILER_HH
#define CORE_COMPILER_HH 1
#pragma once

#if defined(_MSC_VER)
#define PR_DLLEXPORT   __declspec(dllexport)
#define PR_DLLIMPORT   __declspec(dllimport)
#define PR_FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define PR_DLLEXPORT   __attribute__((visibility("default")))
#define PR_DLLIMPORT   __attribute__((visibility("default")))
#define PR_FORCEINLINE inline __attribute__((always_inline))
#else
#define PR_DLLEXPORT
#define PR_DLLIMPORT
#define PR_FORCEINLINE inline
#endif

#endif // CORE_COMPILER_HH
