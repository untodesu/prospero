// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: ed25519.cc; Created: Mon Nov 24 2025 22:45:18
// Description: libed25519 wrapper so nothing else depends on it

#include "core/precompiled.hh"

#include "core/ed25519.hh"

template<std::size_t N>
static std::string export_buffer(const std::array<std::byte, N>& buffer)
{
    std::string hexstring;
    hexstring.resize(2U * N);

    for(std::size_t i = 0U; i < N; ++i) {
        std::snprintf(&hexstring[2U * i], 3U, "%02hhX", static_cast<unsigned char>(buffer[i]));
    }

    return hexstring;
}

template<std::size_t N>
static bool import_buffer(std::string_view hexstring, std::array<std::byte, N>& buffer)
{
    if(hexstring.size() == (2U * N)) {
        for(std::size_t i = 0U; i < N; ++i) {
            if(1 == std::sscanf(&hexstring[2U * i], "%2hhX", reinterpret_cast<unsigned char*>(&buffer[i])))
                continue;
            return false;
        }

        return true;
    }

    return false;
}

CORE_API void core::ed25519::generate_seed(seed_buffer& seed)
{
    auto seed_ptr = reinterpret_cast<unsigned char*>(seed.data());

    ed25519_create_seed(seed_ptr);
}

CORE_API void core::ed25519::generate_keys(const seed_buffer& seed, pkey_buffer& pkey, skey_buffer& skey)
{
    auto seed_ptr = reinterpret_cast<const unsigned char*>(seed.data());
    auto pkey_ptr = reinterpret_cast<unsigned char*>(pkey.data());
    auto skey_ptr = reinterpret_cast<unsigned char*>(skey.data());

    ed25519_create_keypair(pkey_ptr, skey_ptr, seed_ptr);
}

CORE_API void core::ed25519::generate_exch(const skey_buffer& skey, const pkey_buffer& pkey, exch_buffer& exch)
{
    auto skey_ptr = reinterpret_cast<const unsigned char*>(skey.data());
    auto pkey_ptr = reinterpret_cast<const unsigned char*>(pkey.data());
    auto exch_ptr = reinterpret_cast<unsigned char*>(exch.data());

    ed25519_key_exchange(exch_ptr, pkey_ptr, skey_ptr);
}

CORE_API void core::ed25519::sign(const pkey_buffer& pkey, const skey_buffer& skey, std::span<const std::byte> message, sign_buffer& sign)
{
    auto pkey_ptr = reinterpret_cast<const unsigned char*>(pkey.data());
    auto skey_ptr = reinterpret_cast<const unsigned char*>(skey.data());
    auto msg_ptr = reinterpret_cast<const unsigned char*>(message.data());
    auto sign_ptr = reinterpret_cast<unsigned char*>(sign.data());

    ed25519_sign(sign_ptr, msg_ptr, message.size_bytes(), pkey_ptr, skey_ptr);
}

CORE_API bool core::ed25519::verify(const pkey_buffer& pkey, std::span<const std::byte> message, const sign_buffer& sign)
{
    auto pkey_ptr = reinterpret_cast<const unsigned char*>(pkey.data());
    auto msg_ptr = reinterpret_cast<const unsigned char*>(message.data());
    auto sign_ptr = reinterpret_cast<const unsigned char*>(sign.data());

    return static_cast<bool>(ed25519_verify(sign_ptr, msg_ptr, message.size_bytes(), pkey_ptr));
}

CORE_API bool core::ed25519::import_public_key(std::string_view hexstring, pkey_buffer& pkey)
{
    return import_buffer(hexstring, pkey);
}

CORE_API bool core::ed25519::import_private_key(std::string_view hexstring, skey_buffer& skey)
{
    return import_buffer(hexstring, skey);
}

CORE_API std::string core::ed25519::export_public_key(const pkey_buffer& pkey)
{
    return export_buffer(pkey);
}

CORE_API std::string core::ed25519::export_private_key(const skey_buffer& skey)
{
    return export_buffer(skey);
}
