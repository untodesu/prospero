// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: buffer.hh; Created: Mon Nov 24 2025 22:59:32
// Description: QDataStream-esque buffer for de-/serializing data
#ifndef CORE_BUFFER_HH
#define CORE_BUFFER_HH 1
#pragma once

#include "core/aes256.hh"

namespace core
{
class ReadBuffer final {
public:
    CORE_API static ReadBuffer decrypt(core::aes256::context& context, ReadBuffer& buffer);

    ReadBuffer(void) = default;
    CORE_API explicit ReadBuffer(const ReadBuffer& other);
    CORE_API explicit ReadBuffer(const void* data, std::size_t size);
    CORE_API explicit ReadBuffer(std::istream& stream);
    virtual ~ReadBuffer(void) = default;

    CORE_API std::size_t size(void) const;
    CORE_API const std::byte* data(void) const;

    CORE_API void reset(const void* data, std::size_t size);
    CORE_API void reset(std::istream& stream);

    constexpr void rewind(void);
    constexpr bool is_ended(void) const;

    CORE_API void read(void* buffer, std::size_t size);

    template<typename T>
    CORE_API T read(void);

    template<typename T>
    ReadBuffer& operator>>(T& value);

private:
    std::vector<std::byte> m_vector;
    std::size_t m_position;
};
} // namespace core

namespace core
{
class WriteBuffer final {
public:
    CORE_API static WriteBuffer encrypt(core::aes256::context& context, const WriteBuffer& buffer);

    CORE_API WriteBuffer(void) = default;
    CORE_API WriteBuffer(const WriteBuffer& other);
    CORE_API WriteBuffer(WriteBuffer&&) = default;
    virtual ~WriteBuffer(void) = default;

    CORE_API std::size_t size(void) const;
    CORE_API const std::byte* data(void) const;

    CORE_API void reset(void);

    CORE_API void write(const WriteBuffer& other);
    CORE_API void write(const void* data, std::size_t size);

    template<typename T>
    CORE_API void write(const T value);

    template<typename T>
    WriteBuffer& operator<<(const T value);

    CORE_API void to_stream(std::ostream& stream) const;

private:
    std::vector<std::byte> m_vector;
};
} // namespace core

constexpr void core::ReadBuffer::rewind(void)
{
    m_position = 0;
}

constexpr bool core::ReadBuffer::is_ended(void) const
{
    return m_position >= m_vector.size();
}

template<typename T>
core::ReadBuffer& core::ReadBuffer::operator>>(T& value)
{
    value = read<T>();
    return *this;
}

template<typename T>
core::WriteBuffer& core::WriteBuffer::operator<<(const T value)
{
    write<T>(value);
    return *this;
}

#endif // CORE_BUFFER_HH
