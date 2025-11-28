## SPDX-License-Identifier: MPL-2.0
## Copyright (c) 2025 Kirill Dmitrievich
## File: CMakeLists.txt; Created: Tue Nov 25 2025 00:49:31
## Description:

add_subdirectory(aes)
add_subdirectory(ed25519)
add_subdirectory(enet)
add_subdirectory(uulog)

# Generate thirdparty.txt; I find it annoying to manually aggregate
# all the third-party libraries licenses from all over the place so
# this convenient script is going to do that for me automatically
find_package(Python3 COMPONENTS Interpreter REQUIRED)
add_custom_target(thirdparty_txt ALL
    COMMAND ${Python3_EXECUTABLE} "${PROJECT_SOURCE_DIR}/scripts/generate-thirdparty.py" "${PROJECT_SOURCE_DIR}/external"
    BYPRODUCTS "${PROJECT_BINARY_DIR}/thirdparty.txt"
    DEPENDS "${PROJECT_SOURCE_DIR}/scripts/generate-thirdparty.py"
    WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
install(FILES "${PROJECT_BINARY_DIR}/thirdparty.txt" DESTINATION "doc/prospero")
