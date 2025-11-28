## SPDX-License-Identifier: MPL-2.0
## Copyright (c) 2025 Kirill Dmitrievich
## File: CMakeLists.txt; Created: Tue Nov 25 2025 00:49:19
## Description:

add_subdirectory(core)

if(BUILD_CLIENT)
    add_subdirectory(client)
endif()

if(BUILD_SERVER)
    add_subdirectory(server)
endif()
