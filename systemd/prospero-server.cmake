## SPDX-License-Identifier: MPL-2.0
## Copyright (c) 2025 Kirill Dmitrievich
## File: generate-service.cmake; Created: Fri Nov 28 2025 11:10:34
## Description: Systemd servicefile generation

set(INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
set(INSTALL_SYSCONFDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}")

configure_file("${CMAKE_CURRENT_LIST_DIR}/prospero-server@.service.in" "${PROJECT_BINARY_DIR}/generated.prospero-server@.service")
install(FILES "${PROJECT_BINARY_DIR}/generated.prospero-server@service" DESTINATION "lib/systemd/system" RENAME "prospero-server@.service")
install(DIRECTORY DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/prospero")
