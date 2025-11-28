## SPDX-License-Identifier: MPL-2.0
## Copyright (c) 2025 Kirill Dmitrievich
## File: generate-service.cmake; Created: Fri Nov 28 2025 11:10:34
## Description: Systemd servicefile generation

set(INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}/bin")
set(INSTALL_SYSCONFDIR "${CMAKE_INSTALL_PREFIX}/etc")

configure_file("${CMAKE_CURRENT_LIST_DIR}/prospero-server@.service.in" "${PROJECT_BINARY_DIR}/generated.prospero-server@.service")
file(INSTALL "${PROJECT_BINARY_DIR}/generated.prospero-server@.service" DESTINATION "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/systemd/system" RENAME "prospero-server@.service")
file(MAKE_DIRECTORY "$ENV{DESTDIR}/bin/prospero")
