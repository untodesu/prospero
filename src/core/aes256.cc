// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: aes256.cc; Created: Mon Nov 24 2025 22:54:17
// Description: libaes wrapper so only core.dll depends on it

#include "core/precompiled.hh"

#include "core/aes256.hh"

void aes256::create(context& context, const skey_buffer& key)
{
    assert(context == nullptr);

    auto aes_context = new AES_ctx;
    auto key_ptr = reinterpret_cast<const uint8_t*>(key.data());

    AES_init_ctx(aes_context, key_ptr);

    context = reinterpret_cast<void*>(aes_context);
}

void aes256::destroy(context& context)
{
    if(context) {
        auto aes_context = reinterpret_cast<AES_ctx*>(context);

        delete aes_context;

        context = nullptr;
    }
}

void aes256::set_ivec(context& context, const ivec_buffer& ivec)
{
    assert(context);

    auto aes_context = reinterpret_cast<AES_ctx*>(context);
    auto ivec_ptr = reinterpret_cast<const uint8_t*>(ivec.data());

    AES_ctx_set_iv(aes_context, ivec_ptr);
}

void aes256::encrypt_insitu(context& context, std::span<std::byte> data)
{
    assert(context);
    assert(data.size_bytes() % BLOCK_SIZE == 0U);

    auto aes_context = reinterpret_cast<AES_ctx*>(context);
    auto data_ptr = reinterpret_cast<uint8_t*>(data.data());

    AES_CBC_encrypt_buffer(aes_context, data_ptr, data.size_bytes());
}

void aes256::decrypt_insitu(context& context, std::span<std::byte> data)
{
    assert(context);
    assert(data.size_bytes() % BLOCK_SIZE == 0U);

    auto aes_context = reinterpret_cast<AES_ctx*>(context);
    auto data_ptr = reinterpret_cast<uint8_t*>(data.data());

    AES_CBC_decrypt_buffer(aes_context, data_ptr, data.size_bytes());
}
