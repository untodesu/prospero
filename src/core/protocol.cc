// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: protocol.cc; Created: Mon Nov 24 2025 23:49:08
// Description: Packet definitions

#include "core/precompiled.hh"

#include "core/protocol.hh"

#include "core/buffer.hh"

CORE_API void core::AuthChallengeRequest::deserialize(ReadBuffer& buffer, AuthChallengeRequest& packet)
{
    buffer.read(packet.server_pkey.data(), packet.server_pkey.size());
    buffer.read(packet.challenge_data.data(), packet.challenge_data.size());
    packet.challenge_timestamp = buffer.read<std::uint64_t>();
}

CORE_API void core::AuthChallengeRequest::serialize(WriteBuffer& buffer, const AuthChallengeRequest& packet)
{
    buffer.write(packet.server_pkey.data(), packet.server_pkey.size());
    buffer.write(packet.challenge_data.data(), packet.challenge_data.size());
    buffer.write<std::uint64_t>(packet.challenge_timestamp);
}

CORE_API void core::AuthChallengeResponse::deserialize(ReadBuffer& buffer, AuthChallengeResponse& packet)
{
    buffer.read(packet.client_pkey.data(), packet.client_pkey.size());
    buffer.read(packet.signature.data(), packet.signature.size());
    packet.desired_username = buffer.read<std::string>();
}

CORE_API void core::AuthChallengeResponse::serialize(WriteBuffer& buffer, const AuthChallengeResponse& packet)
{
    buffer.write(packet.client_pkey.data(), packet.client_pkey.size());
    buffer.write(packet.signature.data(), packet.signature.size());
    buffer.write<std::string_view>(packet.desired_username);
}

CORE_API void core::AuthChallengeResult::deserialize(ReadBuffer& buffer, AuthChallengeResult& packet)
{
    packet.auth_status = buffer.read<std::uint32_t>();
    packet.assigned_user_id = buffer.read<std::uint64_t>();
    packet.assigned_username = buffer.read<std::string>();
}

CORE_API void core::AuthChallengeResult::serialize(WriteBuffer& buffer, const AuthChallengeResult& packet)
{
    buffer.write<std::uint32_t>(packet.auth_status);
    buffer.write<std::uint64_t>(packet.assigned_user_id);
    buffer.write<std::string_view>(packet.assigned_username);
}

CORE_API void core::ChannelDefinition::deserialize(core::aes256::context& context, ReadBuffer& buffer, ChannelDefinition& packet)
{
    auto payload = ReadBuffer::decrypt(context, buffer);
    packet.channel_id = payload.read<std::uint64_t>();
    packet.channel_name = payload.read<std::string>();
}

CORE_API void core::ChannelDefinition::serialize(core::aes256::context& context, WriteBuffer& buffer, const ChannelDefinition& packet)
{
    WriteBuffer payload;
    payload.write<std::uint64_t>(packet.channel_id);
    payload.write<std::string_view>(packet.channel_name);

    buffer.write(WriteBuffer::encrypt(context, payload));
}

CORE_API void core::ChannelUserJoin::deserialize(core::aes256::context& context, ReadBuffer& buffer, ChannelUserJoin& packet)
{
    auto payload = ReadBuffer::decrypt(context, buffer);
    packet.user_id = payload.read<std::uint64_t>();
    packet.channel_id = payload.read<std::uint64_t>();
    packet.username = payload.read<std::string>();
}

CORE_API void core::ChannelUserJoin::serialize(core::aes256::context& context, WriteBuffer& buffer, const ChannelUserJoin& packet)
{
    WriteBuffer payload;
    payload.write<std::uint64_t>(packet.user_id);
    payload.write<std::uint64_t>(packet.channel_id);
    payload.write<std::string_view>(packet.username);

    buffer.write(WriteBuffer::encrypt(context, payload));
}

CORE_API void core::ChannelUserLeave::deserialize(core::aes256::context& context, ReadBuffer& buffer, ChannelUserLeave& packet)
{
    auto payload = ReadBuffer::decrypt(context, buffer);
    packet.user_id = payload.read<std::uint64_t>();
    packet.channel_id = payload.read<std::uint64_t>();
}

CORE_API void core::ChannelUserLeave::serialize(core::aes256::context& context, WriteBuffer& buffer, const ChannelUserLeave& packet)
{
    WriteBuffer payload;
    payload.write<std::uint64_t>(packet.user_id);
    payload.write<std::uint64_t>(packet.channel_id);

    buffer.write(WriteBuffer::encrypt(context, payload));
}

CORE_API void core::SystemMessage::deserialize(core::aes256::context& context, ReadBuffer& buffer, SystemMessage& packet)
{
    auto payload = ReadBuffer::decrypt(context, buffer);
    packet.severity = payload.read<std::uint32_t>();
    packet.message = payload.read<std::string>();
}

CORE_API void core::SystemMessage::serialize(core::aes256::context& context, WriteBuffer& buffer, const SystemMessage& packet)
{
    WriteBuffer payload;
    payload.write<std::uint32_t>(packet.severity);
    payload.write<std::string_view>(packet.message);

    buffer.write(WriteBuffer::encrypt(context, payload));
}

CORE_API void core::TextMessage::deserialize(core::aes256::context& context, ReadBuffer& buffer, TextMessage& packet)
{
    auto payload = ReadBuffer::decrypt(context, buffer);
    packet.channel_id = payload.read<std::uint64_t>();
    packet.user_id = payload.read<std::uint64_t>();
    packet.message = payload.read<std::string>();
}

CORE_API void core::TextMessage::serialize(core::aes256::context& context, WriteBuffer& buffer, const TextMessage& packet)
{
    WriteBuffer payload;
    payload.write<std::uint64_t>(packet.channel_id);
    payload.write<std::uint64_t>(packet.user_id);
    payload.write<std::string_view>(packet.message);

    buffer.write(WriteBuffer::encrypt(context, payload));
}
