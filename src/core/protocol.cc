// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: protocol.cc; Created: Mon Nov 24 2025 23:49:08
// Description: Packet definitions

#include "core/precompiled.hh"

#include "core/protocol.hh"

#include "core/buffer.hh"

void AuthChallengeRequest::deserialize(ReadBuffer& buffer, AuthChallengeRequest& packet)
{
    buffer.read(packet.challenge_data.data(), packet.challenge_data.size());
    packet.challenge_timestamp = buffer.read<std::uint64_t>();
    packet.protocol_version = buffer.read<std::uint32_t>();
}

void AuthChallengeRequest::serialize(WriteBuffer& buffer, const AuthChallengeRequest& packet)
{
    buffer.write(packet.challenge_data.data(), packet.challenge_data.size());
    buffer.write<std::uint64_t>(packet.challenge_timestamp);
    buffer.write<std::uint32_t>(packet.protocol_version);
}

void AuthChallengeResponse::deserialize(ReadBuffer& buffer, AuthChallengeResponse& packet)
{
    buffer.read(packet.client_pkey.data(), packet.client_pkey.size());
    buffer.read(packet.signature.data(), packet.signature.size());
    packet.username = buffer.read<std::string>();
}

void AuthChallengeResponse::serialize(WriteBuffer& buffer, const AuthChallengeResponse& packet)
{
    buffer.write(packet.client_pkey.data(), packet.client_pkey.size());
    buffer.write(packet.signature.data(), packet.signature.size());
    buffer.write<std::string_view>(packet.username);
}

void AuthChallengeResult::deserialize(ReadBuffer& buffer, AuthChallengeResult& packet)
{
    packet.status = buffer.read<std::uint32_t>();
    buffer.read(packet.server_pkey.data(), packet.server_pkey.size());
    packet.username = buffer.read<std::string>();
}

void AuthChallengeResult::serialize(WriteBuffer& buffer, const AuthChallengeResult& packet)
{
    buffer.write<std::uint32_t>(packet.status);
    buffer.write(packet.server_pkey.data(), packet.server_pkey.size());
    buffer.write<std::string_view>(packet.username);
}

void ChannelDefinition::deserialize(aes256::context& context, ReadBuffer& buffer, ChannelDefinition& packet)
{
    auto payload = ReadBuffer::decrypt(context, buffer);
    packet.name = payload.read<std::string>();
}

void ChannelDefinition::serialize(aes256::context& context, WriteBuffer& buffer, const ChannelDefinition& packet)
{
    WriteBuffer payload;
    payload.write<std::string_view>(packet.name);

    buffer.write(WriteBuffer::encrypt(context, payload));
}

void SystemMessage::deserialize(aes256::context& context, ReadBuffer& buffer, SystemMessage& packet)
{
    auto payload = ReadBuffer::decrypt(context, buffer);
    packet.timestamp = payload.read<std::uint64_t>();
    packet.message = payload.read<std::string>();
}

void SystemMessage::serialize(aes256::context& context, WriteBuffer& buffer, const SystemMessage& packet)
{
    WriteBuffer payload;
    payload.write<std::uint64_t>(packet.timestamp);
    payload.write<std::string_view>(packet.message);

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
    payload.write<std::string_view>(packet.username);
    payload.write<std::string_view>(packet.message);

    buffer.write(WriteBuffer::encrypt(context, payload));
}
