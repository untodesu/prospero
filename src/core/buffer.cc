// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: buffer.cc; Created: Mon Nov 24 2025 23:02:05
// Description: QDataStream-esque buffer for de-/serializing data

#include "core/precompiled.hh"

#include "core/buffer.hh"

#include "core/aes256.hh"
#include "core/ed25519.hh"

CORE_API core::ReadBuffer::ReadBuffer(const ReadBuffer& other)
{
    reset(other.data(), other.size());
}

CORE_API core::ReadBuffer::ReadBuffer(const void* data, std::size_t size)
{
    assert(data);

    reset(data, size);
}

CORE_API core::ReadBuffer::ReadBuffer(std::istream& stream)
{
    assert(stream.good());

    reset(stream);
}

CORE_API std::size_t core::ReadBuffer::size(void) const
{
    return m_vector.size();
}

CORE_API const std::byte* core::ReadBuffer::data(void) const
{
    return m_vector.data();
}

CORE_API void core::ReadBuffer::reset(const void* data, std::size_t size)
{
    assert(data);

    auto bytes = reinterpret_cast<const std::byte*>(data);
    m_vector.assign(bytes, bytes + size);
    m_position = 0U;
}

CORE_API void core::ReadBuffer::reset(std::istream& stream)
{
    assert(stream.good());

    stream.seekg(0, std::ios::end);
    m_vector.resize(static_cast<std::size_t>(stream.tellg()));

    stream.seekg(0, std::ios::beg);
    stream.read(reinterpret_cast<char*>(m_vector.data()), static_cast<std::streamsize>(m_vector.size()));

    m_position = 0U;
}

template<>
CORE_API std::byte core::ReadBuffer::read<std::byte>(void)
{
    if(m_position < m_vector.size()) {
        auto result = m_vector[m_position];
        m_position += 1U;
        return result;
    }

    m_position += 1U;
    return static_cast<std::byte>(0x00);
}

template<>
CORE_API std::uint8_t core::ReadBuffer::read<std::uint8_t>(void)
{
    if((m_position + 1U) <= m_vector.size()) {
        auto result = static_cast<std::uint8_t>(m_vector[m_position]);
        m_position += 1U;
        return result;
    }

    m_position += 1U;
    return 0;
}

template<>
CORE_API std::uint16_t core::ReadBuffer::read<std::uint16_t>(void)
{
    if((m_position + 2U) <= m_vector.size()) {
        auto result = UINT16_C(0x0000);
        result |= (UINT16_C(0x00FF) & static_cast<std::uint16_t>(m_vector[m_position + 0U])) << 8U;
        result |= (UINT16_C(0x00FF) & static_cast<std::uint16_t>(m_vector[m_position + 1U])) << 0U;
        m_position += 2U;
        return result;
    }

    m_position += 2U;
    return 0;
}

template<>
CORE_API std::uint32_t core::ReadBuffer::read<std::uint32_t>(void)
{
    if((m_position + 4U) <= m_vector.size()) {
        auto result = UINT32_C(0x00000000);
        result |= (UINT32_C(0x000000FF) & static_cast<std::uint32_t>(m_vector[m_position + 0U])) << 24U;
        result |= (UINT32_C(0x000000FF) & static_cast<std::uint32_t>(m_vector[m_position + 1U])) << 16U;
        result |= (UINT32_C(0x000000FF) & static_cast<std::uint32_t>(m_vector[m_position + 2U])) << 8U;
        result |= (UINT32_C(0x000000FF) & static_cast<std::uint32_t>(m_vector[m_position + 3U])) << 0U;
        m_position += 4U;
        return result;
    }

    m_position += 4U;
    return 0;
}

template<>
CORE_API std::uint64_t core::ReadBuffer::read<std::uint64_t>(void)
{
    if((m_position + 8U) <= m_vector.size()) {
        auto result = UINT64_C(0x0000000000000000);
        result |= (UINT64_C(0x00000000000000FF) & static_cast<std::uint64_t>(m_vector[m_position + 0U])) << 56U;
        result |= (UINT64_C(0x00000000000000FF) & static_cast<std::uint64_t>(m_vector[m_position + 1U])) << 48U;
        result |= (UINT64_C(0x00000000000000FF) & static_cast<std::uint64_t>(m_vector[m_position + 2U])) << 40U;
        result |= (UINT64_C(0x00000000000000FF) & static_cast<std::uint64_t>(m_vector[m_position + 3U])) << 32U;
        result |= (UINT64_C(0x00000000000000FF) & static_cast<std::uint64_t>(m_vector[m_position + 4U])) << 24U;
        result |= (UINT64_C(0x00000000000000FF) & static_cast<std::uint64_t>(m_vector[m_position + 5U])) << 16U;
        result |= (UINT64_C(0x00000000000000FF) & static_cast<std::uint64_t>(m_vector[m_position + 6U])) << 8U;
        result |= (UINT64_C(0x00000000000000FF) & static_cast<std::uint64_t>(m_vector[m_position + 7U])) << 0U;
        m_position += 8U;
        return result;
    }

    m_position += 8U;
    return 0;
}

template<>
CORE_API float core::ReadBuffer::read<float>(void)
{
    return std::bit_cast<float>(read<std::uint32_t>());
}

template<>
CORE_API std::int8_t core::ReadBuffer::read<std::int8_t>(void)
{
    return std::bit_cast<std::int8_t>(read<std::uint8_t>());
}

template<>
CORE_API std::int16_t core::ReadBuffer::read<std::int16_t>(void)
{
    return std::bit_cast<std::int16_t>(read<std::uint16_t>());
}

template<>
CORE_API std::int32_t core::ReadBuffer::read<std::int32_t>(void)
{
    return std::bit_cast<std::int32_t>(read<std::uint32_t>());
}

template<>
CORE_API std::int64_t core::ReadBuffer::read<std::int64_t>(void)
{
    return std::bit_cast<std::int64_t>(read<std::uint64_t>());
}

template<>
CORE_API std::string core::ReadBuffer::read<std::string>(void)
{
    std::string result;
    result.resize(read<std::uint16_t>());

    for(std::size_t i = 0; i < result.size(); ++i) {
        if(m_position < m_vector.size()) {
            result[i] = static_cast<char>(m_vector[m_position]);
        }

        m_position += 1U;
    }

    return result;
}

CORE_API void core::ReadBuffer::read(void* buffer, std::size_t size)
{
    auto bytes = reinterpret_cast<std::byte*>(buffer);
    auto amount_to_read = std::min(size, m_vector.size() - m_position);

    if(amount_to_read > 0) {
        std::copy(m_vector.cbegin() + m_position, m_vector.cbegin() + m_position + amount_to_read, bytes);
    }

    m_position += size;
}

CORE_API core::ReadBuffer core::ReadBuffer::decrypt(core::aes256::context& context, ReadBuffer& buffer)
{
    core::aes256::ivec_buffer ivec;
    buffer.read(ivec.data(), ivec.size());

    std::vector<std::byte> payload;
    payload.resize(buffer.read<std::uint32_t>());
    buffer.read(payload.data(), payload.size());

    assert(buffer.size() % core::aes256::BLOCK_SIZE == 0U);

    core::aes256::set_ivec(context, ivec);
    core::aes256::decrypt_insitu(context, payload);

    return core::ReadBuffer(payload.data(), payload.size());
}

CORE_API core::WriteBuffer::WriteBuffer(const WriteBuffer& other)
{
    m_vector = other.m_vector;
}

CORE_API std::size_t core::WriteBuffer::size(void) const
{
    return m_vector.size();
}

CORE_API const std::byte* core::WriteBuffer::data(void) const
{
    return m_vector.data();
}

CORE_API void core::WriteBuffer::reset(void)
{
    m_vector.clear();
}

CORE_API void core::WriteBuffer::write(const WriteBuffer& other)
{
    m_vector.insert(m_vector.end(), other.m_vector.begin(), other.m_vector.end());
}

CORE_API void core::WriteBuffer::write(const void* data, std::size_t size)
{
    assert(data);

    auto bytes = reinterpret_cast<const std::byte*>(data);
    m_vector.insert(m_vector.end(), bytes, bytes + size);
}

template<>
CORE_API void core::WriteBuffer::write<std::byte>(const std::byte value)
{
    m_vector.push_back(value);
}

template<>
CORE_API void core::WriteBuffer::write<std::uint8_t>(const std::uint8_t value)
{
    m_vector.push_back(static_cast<std::byte>(value));
}

template<>
CORE_API void core::WriteBuffer::write<std::uint16_t>(const std::uint16_t value)
{
    m_vector.push_back(static_cast<std::byte>(UINT16_C(0xFF) & ((value & UINT16_C(0xFF00)) >> 8U)));
    m_vector.push_back(static_cast<std::byte>(UINT16_C(0xFF) & ((value & UINT16_C(0x00FF)) >> 0U)));
}

template<>
CORE_API void core::WriteBuffer::write<std::uint32_t>(const std::uint32_t value)
{
    m_vector.push_back(static_cast<std::byte>(UINT32_C(0xFF) & ((value & UINT32_C(0xFF000000)) >> 24U)));
    m_vector.push_back(static_cast<std::byte>(UINT32_C(0xFF) & ((value & UINT32_C(0x00FF0000)) >> 16U)));
    m_vector.push_back(static_cast<std::byte>(UINT32_C(0xFF) & ((value & UINT32_C(0x0000FF00)) >> 8U)));
    m_vector.push_back(static_cast<std::byte>(UINT32_C(0xFF) & ((value & UINT32_C(0x000000FF)) >> 0U)));
}

template<>
CORE_API void core::WriteBuffer::write<std::uint64_t>(const std::uint64_t value)
{
    m_vector.push_back(static_cast<std::byte>(UINT64_C(0xFF) & ((value & UINT64_C(0xFF00000000000000)) >> 56U)));
    m_vector.push_back(static_cast<std::byte>(UINT64_C(0xFF) & ((value & UINT64_C(0x00FF000000000000)) >> 48U)));
    m_vector.push_back(static_cast<std::byte>(UINT64_C(0xFF) & ((value & UINT64_C(0x0000FF0000000000)) >> 40U)));
    m_vector.push_back(static_cast<std::byte>(UINT64_C(0xFF) & ((value & UINT64_C(0x000000FF00000000)) >> 32U)));
    m_vector.push_back(static_cast<std::byte>(UINT64_C(0xFF) & ((value & UINT64_C(0x00000000FF000000)) >> 24U)));
    m_vector.push_back(static_cast<std::byte>(UINT64_C(0xFF) & ((value & UINT64_C(0x0000000000FF0000)) >> 16U)));
    m_vector.push_back(static_cast<std::byte>(UINT64_C(0xFF) & ((value & UINT64_C(0x000000000000FF00)) >> 8U)));
    m_vector.push_back(static_cast<std::byte>(UINT64_C(0xFF) & ((value & UINT64_C(0x00000000000000FF)) >> 0U)));
}

template<>
CORE_API void core::WriteBuffer::write(const float value)
{
    write(std::bit_cast<std::uint32_t>(value));
}

template<>
CORE_API void core::WriteBuffer::write(const std::int8_t value)
{
    write(std::bit_cast<std::uint8_t>(value));
}

template<>
CORE_API void core::WriteBuffer::write(const std::int16_t value)
{
    write(std::bit_cast<std::uint16_t>(value));
}

template<>
CORE_API void core::WriteBuffer::write(const std::int32_t value)
{
    write(std::bit_cast<std::uint32_t>(value));
}

template<>
CORE_API void core::WriteBuffer::write(const std::int64_t value)
{
    write(std::bit_cast<std::uint64_t>(value));
}

template<>
CORE_API void core::WriteBuffer::write<std::string_view>(const std::string_view value)
{
    write<std::uint16_t>(static_cast<std::uint16_t>(value.size()));

    for(const auto& character : value) {
        m_vector.push_back(static_cast<std::byte>(character));
    }
}

CORE_API void core::WriteBuffer::to_stream(std::ostream& stream) const
{
    assert(stream.good());

    stream.write(reinterpret_cast<const char*>(m_vector.data()), static_cast<std::streamsize>(m_vector.size()));
}

CORE_API core::WriteBuffer core::WriteBuffer::encrypt(core::aes256::context& context, const core::WriteBuffer& buffer)
{
    core::ed25519::seed_buffer seed;
    core::ed25519::generate_seed(seed);

    core::aes256::ivec_buffer ivec;
    std::copy(seed.begin(), seed.begin() + ivec.size(), ivec.begin());

    std::vector<std::byte> payload;
    payload.insert(payload.begin(), buffer.data(), buffer.data() + buffer.size());
    payload.resize(((payload.size() / AES_BLOCKLEN) + 1) * AES_BLOCKLEN, static_cast<std::byte>(0));

    core::aes256::set_ivec(context, ivec);
    core::aes256::encrypt_insitu(context, payload);

    core::WriteBuffer output;
    output.write(ivec.data(), ivec.size());
    output.write<std::uint32_t>(static_cast<std::uint32_t>(payload.size()));
    output.write(payload.data(), payload.size());

    return output;
}
