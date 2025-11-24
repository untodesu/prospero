// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: host.hh; Created: Tue Nov 25 2025 00:44:08
// Description: Network management

#ifndef SERVER_HOST_HH
#define SERVER_HOST_HH 1
#pragma once

namespace host
{
void init(void);
void init_late(void);
void shutdown(void);
void update(void);
} // namespace host

#endif // SERVER_HOST_HH
