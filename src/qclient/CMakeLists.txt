qt_add_executable(prospero
    "${PROSPERO_INCLUDES}/qclient/clipboard_wrap.hh"
    "${PROSPERO_INCLUDES}/qclient/precompiled.hh"
    "${PROSPERO_INCLUDES}/qclient/version_wrap.hh"
    "${PROSPERO_SOURCES}/qclient/clipboard_wrap.cc"
    "${PROSPERO_SOURCES}/qclient/main.cc"
    "${PROSPERO_SOURCES}/qclient/version_wrap.cc")
target_compile_features(prospero PUBLIC cxx_std_20)
target_include_directories(prospero PUBLIC "${PROSPERO_INCLUDES}")
target_include_directories(prospero PRIVATE "${PROSPERO_SOURCES}")
target_link_libraries(prospero PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickControls2 Qt6::Widgets)
target_link_libraries(prospero PRIVATE core enet)

target_precompile_headers(prospero PRIVATE "${PROSPERO_INCLUDES}/qclient/precompiled.hh")

file(GLOB qml_files CONFIGURE_DEPENDS "${PROSPERO_SOURCES}/qclient/qml/*.qml")
list(TRANSFORM qml_files REPLACE "${PROSPERO_SOURCES}/qclient/" "")

file(GLOB res_files CONFIGURE_DEPENDS "${PROSPERO_SOURCES}/qclient/res/*")
list(TRANSFORM res_files REPLACE "${PROSPERO_SOURCES}/qclient/" "")

qt_add_qml_module(prospero URI prospero VERSION 1.0 QML_FILES ${qml_files})

qt_add_resources(prospero qml_files PREFIX / FILES ${qml_files})
qt_add_resources(prospero res_files PREFIX / FILES ${res_files})
