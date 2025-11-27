## SPDX-License-Identifier: MPL-2.0
## Copyright (c) 2025 Kirill Dmitrievich
## File: CMakeLists.txt; Created: Tue Nov 25 2025 11:41:39
## Description: QML client executable build configuration

qt_add_executable(prospero
    "${CMAKE_CURRENT_LIST_DIR}/clipboard.cc"
    "${CMAKE_CURRENT_LIST_DIR}/clipboard.hh"
    "${CMAKE_CURRENT_LIST_DIR}/ipc.cc"
    "${CMAKE_CURRENT_LIST_DIR}/ipc.hh"
    "${CMAKE_CURRENT_LIST_DIR}/main.cc"
    "${CMAKE_CURRENT_LIST_DIR}/precompiled.hh"
    "${CMAKE_CURRENT_LIST_DIR}/recents.cc"
    "${CMAKE_CURRENT_LIST_DIR}/recents.hh"
    "${CMAKE_CURRENT_LIST_DIR}/session.cc"
    "${CMAKE_CURRENT_LIST_DIR}/session.hh"
    "${CMAKE_CURRENT_LIST_DIR}/settings.cc"
    "${CMAKE_CURRENT_LIST_DIR}/settings.hh"
    "${CMAKE_CURRENT_LIST_DIR}/version.cc"
    "${CMAKE_CURRENT_LIST_DIR}/version.hh")
target_compile_features(prospero PUBLIC cxx_std_20)
target_include_directories(prospero PUBLIC "${PROSPERO_INCLUDES}")
target_include_directories(prospero PRIVATE "${PROSPERO_SOURCES}")
target_link_libraries(prospero PRIVATE
    Qt6::Concurrent
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Widgets)
target_link_libraries(prospero PRIVATE core enet)

target_precompile_headers(prospero PRIVATE "${CMAKE_CURRENT_LIST_DIR}/precompiled.hh")

set_target_properties(prospero PROPERTIES WIN32_EXECUTABLE TRUE)
set_target_properties(prospero PROPERTIES MACOSX_BUNDLE TRUE)

file(GLOB_RECURSE qml_files CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/qml/*.qml")
list(TRANSFORM qml_files REPLACE "${CMAKE_CURRENT_LIST_DIR}/" "")

file(GLOB_RECURSE res_files CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/res/*")
list(TRANSFORM res_files REPLACE "${CMAKE_CURRENT_LIST_DIR}/" "")

qt_add_qml_module(prospero URI prospero VERSION 1.0 QML_FILES ${qml_files})

qt_add_resources(prospero qml_files PREFIX / FILES ${qml_files})
qt_add_resources(prospero res_files PREFIX / FILES ${res_files})

if(WIN32)
    enable_language(RC)
    target_sources(prospero PRIVATE "${CMAKE_CURRENT_LIST_DIR}/prospero.rc")
endif()

install(TARGETS prospero RUNTIME DESTINATION bin)

qt_generate_deploy_qml_app_script(TARGET prospero OUTPUT_SCRIPT prospero_deploy_script)
install(SCRIPT ${prospero_deploy_script})
