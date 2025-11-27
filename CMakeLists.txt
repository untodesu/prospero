## SPDX-License-Identifier: MPL-2.0
## Copyright (c) 2025 Kirill Dmitrievich
## File: CMakeLists.txt; Created: Tue Nov 25 2025 00:50:26
## Description: root CMake configuration

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(Prospero VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0100 NEW)
set(CMAKE_AUTOMOC ON CACHE BOOL "" FORCE)
set(CMAKE_AUTORCC ON CACHE BOOL "" FORCE)
set(CMAKE_AUTOUIC ON CACHE BOOL "" FORCE)

# set(QT_QML_GENERATE_QMLLS_INI ON CACHE BOOL "" FORCE)

file(TO_CMAKE_PATH "$ENV{QTDIR}" QT_INSTALL_ROOT)
list(APPEND CMAKE_PREFIX_PATH "${QT_INSTALL_ROOT}/lib/cmake")
find_package(Qt6 REQUIRED COMPONENTS
    Concurrent
    Core
    Gui
    Network
    Qml
    Quick
    QuickControls2
    Widgets)
qt_standard_project_setup(REQUIRES 6.9)

if(MSVC)
    add_compile_options(/MP)
    add_compile_options(/W3)
endif()

execute_process(COMMAND git rev-parse --short=8 HEAD WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE GIT_COMMIT OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
execute_process(COMMAND git rev-parse --abbrev-ref HEAD WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE GIT_BRANCH OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)

add_subdirectory(external)
add_subdirectory(src)

if(UNIX)
    install(FILES "${PROJECT_SOURCE_DIR}/systemd/prospero@.service" DESTINATION "etc/systemd/system" RENAME "prospero@.service")
endif()

install(FILES "${PROJECT_SOURCE_DIR}/LICENSE" DESTINATION ".")

set(CPACK_PACKAGE_NAME "Prospero")
set(CPACK_PACKAGE_VENDOR "untodesu")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}-${GIT_COMMIT}")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/nsis/prospero.ico")
set(CPACK_NSIS_MENU_LINKS "bin/prospero.exe" "Prospero")

include(CPack)
