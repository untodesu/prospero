// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: protocol.cc; Created: Mon Nov 24 2025 23:49:08
// Description: Packet definitions

#include "core/precompiled.hh"

#include "core/protocol.hh"

#include "core/buffer.hh"

void AuthRequest::deserialize(ReadBuffer& buffer, AuthRequest& packet)
{
    packet.version_major = buffer.read<std::uint32_t>();
    packet.version_minor = buffer.read<std::uint32_t>();
    packet.version_patch = buffer.read<std::uint32_t>();
    packet.auth_timestamp = buffer.read<std::uint64_t>();
    buffer.read(packet.challenge.data(), packet.challenge.size());
}

void AuthRequest::serialize(WriteBuffer& buffer, const AuthRequest& packet)
{
    buffer.write<std::uint32_t>(packet.version_major);
    buffer.write<std::uint32_t>(packet.version_minor);
    buffer.write<std::uint32_t>(packet.version_patch);
    buffer.write<std::uint64_t>(packet.auth_timestamp);
    buffer.write(packet.challenge.data(), packet.challenge.size());
}

void AuthResponse::deserialize(ReadBuffer& buffer, AuthResponse& packet)
{
    buffer.read(packet.public_key.data(), packet.public_key.size());
    buffer.read(packet.signature.data(), packet.signature.size());
    packet.desired_username = buffer.read<std::string>();
}

void AuthResponse::serialize(WriteBuffer& buffer, const AuthResponse& packet)
{
    buffer.write(packet.public_key.data(), packet.public_key.size());
    buffer.write(packet.signature.data(), packet.signature.size());
    buffer.write<std::string_view>(packet.desired_username);
}

void AuthResult::deserialize(ReadBuffer& buffer, AuthResult& packet)
{
    buffer.read(packet.public_key.data(), packet.public_key.size());
    packet.assigned_username = buffer.read<std::string>();
}

void AuthResult::serialize(WriteBuffer& buffer, const AuthResult& packet)
{
    buffer.write(packet.public_key.data(), packet.public_key.size());
    buffer.write<std::string_view>(packet.assigned_username);
}

void Notification::deserialize(aes256::context& context, ReadBuffer& buffer, Notification& packet)
{
    auto payload = ReadBuffer::decrypt(context, buffer);
    packet.timestamp = payload.read<std::uint64_t>();
    packet.type = payload.read<std::uint32_t>();
    packet.text_1 = payload.read<std::string>();
    packet.text_2 = payload.read<std::string>();
}

void Notification::serialize(aes256::context& context, WriteBuffer& buffer, const Notification& packet)
{
    WriteBuffer payload;
    payload.write<std::uint64_t>(packet.timestamp);
    payload.write<std::uint32_t>(packet.type);
    payload.write<std::string_view>(packet.text_1);
    payload.write<std::string_view>(packet.text_2);

    buffer.write(WriteBuffer::encrypt(context, payload));
}

void TextMessage::deserialize(aes256::context& context, ReadBuffer& buffer, TextMessage& packet)
{
    auto payload = ReadBuffer::decrypt(context, buffer);
    packet.timestamp = payload.read<std::uint64_t>();
    packet.username = payload.read<std::string>();
    packet.message = payload.read<std::string>();
}

void TextMessage::serialize(aes256::context& context, WriteBuffer& buffer, const TextMessage& packet)
{
    WriteBuffer payload;
    payload.write<std::uint64_t>(packet.timestamp);
    payload.write<std::string_view>(packet.username.substr(0U, MAX_USERNAME_LENGTH));
    payload.write<std::string_view>(packet.message.substr(0U, MAX_MESSAGE_LENGTH));

    buffer.write(WriteBuffer::encrypt(context, payload));
}

void ImageMessage::deserialize(aes256::context& context, ReadBuffer& buffer, ImageMessage& packet)
{
    auto payload = ReadBuffer::decrypt(context, buffer);
    packet.timestamp = payload.read<std::uint64_t>();
    packet.username = payload.read<std::string>();
    packet.source = payload.read<std::string>();
}

void ImageMessage::serialize(aes256::context& context, WriteBuffer& buffer, const ImageMessage& packet)
{
    WriteBuffer payload;
    payload.write<std::uint64_t>(packet.timestamp);
    payload.write<std::string_view>(packet.username.substr(0U, MAX_USERNAME_LENGTH));
    payload.write<std::string_view>(packet.source);

    buffer.write(WriteBuffer::encrypt(context, payload));
}
