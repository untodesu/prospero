// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: identity.hh; Created: Mon Nov 24 2025 23:29:37
// Description: Server ed25519 keypair

#ifndef SERVER_IDENTITY_HH
#define SERVER_IDENTITY_HH 1
#pragma once

#include "core/ed25519.hh"

namespace identity
{
extern core::ed25519::pkey_buffer public_key;
extern core::ed25519::skey_buffer private_key;
} // namespace identity

namespace identity
{
void init(const std::filesystem::path& config_directory);
} // namespace identity

#endif // SERVER_IDENTITY_HH
