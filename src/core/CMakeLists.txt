## SPDX-License-Identifier: MPL-2.0
## Copyright (c) 2025 Kirill Dmitrievich
## File: CMakeLists.txt; Created: Mon Nov 24 2025 23:07:14
## Description: CMake configuration for the core module

if(PROSPERO_STATIC)
    set(CORE_LIB_TYPE STATIC)
else()
    set(CORE_LIB_TYPE SHARED)
endif()

add_library(core ${CORE_LIB_TYPE}
    "${PROSPERO_INCLUDES}/core/aes256.hh"
    "${PROSPERO_INCLUDES}/core/buffer.hh"
    "${PROSPERO_INCLUDES}/core/compiler.hh"
    "${PROSPERO_INCLUDES}/core/concepts.hh"
    "${PROSPERO_INCLUDES}/core/config.hh"
    "${PROSPERO_INCLUDES}/core/crc64ecma.hh"
    "${PROSPERO_INCLUDES}/core/ed25519.hh"
    "${PROSPERO_INCLUDES}/core/exception.hh"
    "${PROSPERO_INCLUDES}/core/exports.hh"
    "${PROSPERO_INCLUDES}/core/precompiled.hh"
    "${PROSPERO_INCLUDES}/core/string.hh"
    "${PROSPERO_INCLUDES}/core/unixtime.hh"
    "${PROSPERO_INCLUDES}/core/version.hh"
    "${PROSPERO_SOURCES}/core/aes256.cc"
    "${PROSPERO_SOURCES}/core/buffer.cc"
    "${PROSPERO_SOURCES}/core/config.cc"
    "${PROSPERO_SOURCES}/core/crc64ecma.cc"
    "${PROSPERO_SOURCES}/core/ed25519.cc"
    "${PROSPERO_SOURCES}/core/protocol.cc"
    "${PROSPERO_SOURCES}/core/string.cc"
    "${PROSPERO_SOURCES}/core/unixtime.cc")
target_compile_features(core PUBLIC cxx_std_20)
target_compile_definitions(core PRIVATE CORE_DLLPRIVATE)
target_include_directories(core PUBLIC "${PROSPERO_INCLUDES}")
target_include_directories(core PRIVATE "${PROSPERO_SOURCES}")
target_link_libraries(core PRIVATE aes ed25519)

target_precompile_headers(core PRIVATE "${PROSPERO_INCLUDES}/core/precompiled.hh")

configure_file("${PROSPERO_SOURCES}/core/version.cc.in" "${PROJECT_BINARY_DIR}/generated.version.cc")
target_sources(core PRIVATE "${PROJECT_BINARY_DIR}/generated.version.cc")

if(MSVC)
    target_compile_definitions(core PUBLIC _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(core PUBLIC _CRT_NONSTDC_NO_WARNINGS)
endif()

if(WIN32)
    target_compile_definitions(core PUBLIC NOMINMAX)
endif()
