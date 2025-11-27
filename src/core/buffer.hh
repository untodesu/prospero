// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: buffer.hh; Created: Mon Nov 24 2025 22:59:32
// Description: QDataStream-esque buffer for de-/serializing data

#ifndef CORE_BUFFER_HH
#define CORE_BUFFER_HH 1
#pragma once

#include "core/aes256.hh"

class ReadBuffer final {
public:
    static ReadBuffer decrypt(aes256::context& context, ReadBuffer& buffer);

    ReadBuffer(void) = default;
    explicit ReadBuffer(const ReadBuffer& other);
    explicit ReadBuffer(const void* data, std::size_t size);
    explicit ReadBuffer(std::istream& stream);
    virtual ~ReadBuffer(void) = default;

    std::size_t size(void) const;
    const std::byte* data(void) const;

    void reset(const void* data, std::size_t size);
    void reset(std::istream& stream);

    constexpr void rewind(void);
    constexpr bool is_ended(void) const;

    void read(void* buffer, std::size_t size);

    template<typename T>
    T read(void);

    template<typename T>
    ReadBuffer& operator>>(T& value);

private:
    std::vector<std::byte> m_vector;
    std::size_t m_position { 0 };
};

class WriteBuffer final {
public:
    static WriteBuffer encrypt(aes256::context& context, const WriteBuffer& buffer);

    WriteBuffer(void) = default;
    WriteBuffer(const WriteBuffer& other);
    WriteBuffer(WriteBuffer&&) = default;
    virtual ~WriteBuffer(void) = default;

    std::size_t size(void) const;
    const std::byte* data(void) const;

    void reset(void);

    void write(const WriteBuffer& other);
    void write(const void* data, std::size_t size);

    template<typename T>
    void write(const T value);

    template<typename T>
    WriteBuffer& operator<<(const T value);

    void to_stream(std::ostream& stream) const;

private:
    std::vector<std::byte> m_vector;
};

constexpr void ReadBuffer::rewind(void)
{
    m_position = 0;
}

constexpr bool ReadBuffer::is_ended(void) const
{
    return m_position >= m_vector.size();
}

template<typename T>
ReadBuffer& ReadBuffer::operator>>(T& value)
{
    value = read<T>();
    return *this;
}

template<typename T>
WriteBuffer& WriteBuffer::operator<<(const T value)
{
    write<T>(value);
    return *this;
}

#endif
