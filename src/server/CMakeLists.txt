## SPDX-License-Identifier: MPL-2.0
## Copyright (c) 2025 Kirill Dmitrievich
## File: CMakeLists.txt; Created: Tue Nov 25 2025 00:47:23
## Description: CMake configuration for the server module

add_executable(prserver
    "${PROSPERO_INCLUDES}/server/channels.hh"
    "${PROSPERO_INCLUDES}/server/host.hh"
    "${PROSPERO_INCLUDES}/server/identity.hh"
    "${PROSPERO_INCLUDES}/server/precompiled.hh"
    "${PROSPERO_INCLUDES}/server/sessions.hh"
    "${PROSPERO_INCLUDES}/server/settings.hh"
    "${PROSPERO_INCLUDES}/server/userlist.hh"
    "${PROSPERO_SOURCES}/server/channels.cc"
    "${PROSPERO_SOURCES}/server/host.cc"
    "${PROSPERO_SOURCES}/server/identity.cc"
    "${PROSPERO_SOURCES}/server/main.cc"
    "${PROSPERO_SOURCES}/server/sessions.cc"
    "${PROSPERO_SOURCES}/server/settings.cc"
    "${PROSPERO_SOURCES}/server/userlist.cc")
target_compile_features(prserver PUBLIC cxx_std_20)
target_include_directories(prserver PUBLIC "${PROSPERO_INCLUDES}")
target_include_directories(prserver PRIVATE "${PROSPERO_SOURCES}")
target_link_libraries(prserver PRIVATE core enet uulog)

target_precompile_headers(prserver PRIVATE "${PROSPERO_INCLUDES}/server/precompiled.hh")
