## SPDX-License-Identifier: MPL-2.0
## Copyright (c) 2025 Kirill Dmitrievich
## File: CMakeLists.txt; Created: Tue Nov 25 2025 00:47:23
## Description: Server executable build configuration

add_executable(prserver
    "${CMAKE_CURRENT_LIST_DIR}/channels.hh"
    "${CMAKE_CURRENT_LIST_DIR}/host.hh"
    "${CMAKE_CURRENT_LIST_DIR}/identity.hh"
    "${CMAKE_CURRENT_LIST_DIR}/precompiled.hh"
    "${CMAKE_CURRENT_LIST_DIR}/sessions.hh"
    "${CMAKE_CURRENT_LIST_DIR}/settings.hh"
    "${CMAKE_CURRENT_LIST_DIR}/userlist.hh"
    "${CMAKE_CURRENT_LIST_DIR}/channels.cc"
    "${CMAKE_CURRENT_LIST_DIR}/host.cc"
    "${CMAKE_CURRENT_LIST_DIR}/identity.cc"
    "${CMAKE_CURRENT_LIST_DIR}/main.cc"
    "${CMAKE_CURRENT_LIST_DIR}/sessions.cc"
    "${CMAKE_CURRENT_LIST_DIR}/settings.cc"
    "${CMAKE_CURRENT_LIST_DIR}/userlist.cc")
target_compile_features(prserver PUBLIC cxx_std_20)
target_include_directories(prserver PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(prserver PRIVATE core enet uulog)

target_precompile_headers(prserver PRIVATE "${CMAKE_CURRENT_LIST_DIR}/precompiled.hh")
