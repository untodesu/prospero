## SPDX-License-Identifier: MPL-2.0
## Copyright (c) 2025 Kirill Dmitrievich
## File: CMakeLists.txt; Created: Mon Nov 24 2025 23:07:14
## Description: Core library build configuration

add_library(core STATIC
    "${CMAKE_CURRENT_LIST_DIR}/aes256.cc"
    "${CMAKE_CURRENT_LIST_DIR}/aes256.hh"
    "${CMAKE_CURRENT_LIST_DIR}/buffer.cc"
    "${CMAKE_CURRENT_LIST_DIR}/buffer.hh"
    "${CMAKE_CURRENT_LIST_DIR}/config.cc"
    "${CMAKE_CURRENT_LIST_DIR}/config.hh"
    "${CMAKE_CURRENT_LIST_DIR}/crc64ecma.cc"
    "${CMAKE_CURRENT_LIST_DIR}/crc64ecma.hh"
    "${CMAKE_CURRENT_LIST_DIR}/ed25519.cc"
    "${CMAKE_CURRENT_LIST_DIR}/ed25519.hh"
    "${CMAKE_CURRENT_LIST_DIR}/exception.hh"
    "${CMAKE_CURRENT_LIST_DIR}/precompiled.hh"
    "${CMAKE_CURRENT_LIST_DIR}/protocol.cc"
    "${CMAKE_CURRENT_LIST_DIR}/protocol.hh"
    "${CMAKE_CURRENT_LIST_DIR}/strtools.cc"
    "${CMAKE_CURRENT_LIST_DIR}/strtools.hh"
    "${CMAKE_CURRENT_LIST_DIR}/unixtime.cc"
    "${CMAKE_CURRENT_LIST_DIR}/unixtime.hh"
    "${CMAKE_CURRENT_LIST_DIR}/version.hh")
target_compile_features(core PUBLIC cxx_std_20)
target_include_directories(core PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(core PRIVATE aes ed25519)

target_precompile_headers(core PRIVATE "${CMAKE_CURRENT_LIST_DIR}/precompiled.hh")

configure_file("${CMAKE_CURRENT_LIST_DIR}/version.cc.in" "${PROJECT_BINARY_DIR}/generated.version.cc")
target_sources(core PRIVATE "${PROJECT_BINARY_DIR}/generated.version.cc")

if(MSVC)
    target_compile_definitions(core PUBLIC _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(core PUBLIC _CRT_NONSTDC_NO_WARNINGS)
endif()

if(WIN32)
    target_compile_definitions(core PUBLIC NOMINMAX)
endif()
