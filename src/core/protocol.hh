// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: protocol.hh; Created: Mon Nov 24 2025 23:34:49
// Description: Packet definitions

#ifndef CORE_PROTOCOL_HH
#define CORE_PROTOCOL_HH 1
#pragma once

#include "core/aes256.hh"
#include "core/ed25519.hh"

constexpr static std::uint32_t PROTOCOL_VERSION = 1U;  ///< Version value passed through ENetPeer
constexpr static std::uint32_t PROTOCOL_MAXCHAN = 64U; ///< Maximum number of channels per server minus one for authentications
constexpr static std::uint32_t PROTOCOL_AUTHCHAN = 0U; ///< Channel used for authentication packets

static_assert(PROTOCOL_MAXCHAN >= 1U);

class ReadBuffer;
class WriteBuffer;

enum class PacketType : std::uint32_t {
    AuthChallengeRequest = 1U,
    AuthChallengeResponse,
    AuthChallengeResult,
    ChannelDefinition,
    SystemMessage,
    TextMessage,
};

template<PacketType Type>
struct BasePacket {
    constexpr static std::uint32_t ID = static_cast<std::uint32_t>(Type);
};

/// Sent on PROTOCOL_AUTHCHAN by the server as soon as a new peer connects;
/// Contains the server's public key and a random challenge nonce the
/// client is supposed to sign and respond with a signature and some other stuff
struct AuthChallengeRequest final : public BasePacket<PacketType::AuthChallengeRequest> {
    static void deserialize(ReadBuffer& buffer, AuthChallengeRequest& packet);
    static void serialize(WriteBuffer& buffer, const AuthChallengeRequest& packet);

    ed25519::seed_buffer challenge_data; ///< Random nonce to be signed by the client
    std::uint64_t challenge_timestamp;   ///< Challenge timestamp in UTC UNIX milliseconds
    std::uint32_t protocol_version;      ///< Protocol version expected by the server
};

/// Sent on PROTOCOL_AUTHCHAN by the client as a response to AuthChallengeRequest;
/// Contains the client's public key, signature of the challenge nonce, and requested username
struct AuthChallengeResponse final : public BasePacket<PacketType::AuthChallengeResponse> {
    static void deserialize(ReadBuffer& buffer, AuthChallengeResponse& packet);
    static void serialize(WriteBuffer& buffer, const AuthChallengeResponse& packet);

    ed25519::pkey_buffer client_pkey; ///< Client's ed25519 public key
    ed25519::sign_buffer signature;   ///< Signature of the challenge_data from AuthChallengeRequest
    std::string username;             ///< Desired username for the session
};

/// Sent on PROTOCOL_AUTHCHAN by the server as a response to AuthChallengeResponse;
/// The final decision on whether to keep the client or drop it is communicated through this packet
struct AuthChallengeResult final : public BasePacket<PacketType::AuthChallengeResult> {
    constexpr static std::uint32_t E_OK = 0x00000000U;    ///< Authentication successful
    constexpr static std::uint32_t E_CRED = 0x00000001U;  ///< Invalid credentials
    constexpr static std::uint32_t E_TIME = 0x00000002U;  ///< Authentication timed out
    constexpr static std::uint32_t E_UNREC = 0x00000003U; ///< Unrecognized public key

    static void deserialize(ReadBuffer& buffer, AuthChallengeResult& packet);
    static void serialize(WriteBuffer& buffer, const AuthChallengeResult& packet);

    std::uint32_t status;             ///< Authentication result
    ed25519::pkey_buffer server_pkey; ///< Server's ed25519 public key
    std::string username;             ///< Assigned username on E_OK, empty otherwise
};

/// Sent on specific channel (so the channel ID is inferred through ENet's API) by the server
/// to update clients about a channel's definition; if no packet is sent on a specific channel, the
/// client should assume the channel with that ID is reserved and is not to be communicated through
struct ChannelDefinition final : public BasePacket<PacketType::ChannelDefinition> {
    static void deserialize(aes256::context& context, ReadBuffer& buffer, ChannelDefinition& packet);
    static void serialize(aes256::context& context, WriteBuffer& buffer, const ChannelDefinition& packet);

    std::string name; ///< Channel name
};

/// Sent on a specific channel by the server to broadcast a system message for all the
/// clients currently present in the channel; this can be used as a response to admin commands
struct SystemMessage final : public BasePacket<PacketType::SystemMessage> {
    static void deserialize(aes256::context& context, ReadBuffer& buffer, SystemMessage& packet);
    static void serialize(aes256::context& context, WriteBuffer& buffer, const SystemMessage& packet);

    std::uint64_t timestamp; ///< Message timestamp in UTC UNIX milliseconds
    std::string message;     ///< System message text
};

/// Sent on a specific channel by a client to broadcast a text message to all the
/// other clients currently present in the channel
struct TextMessage final : public BasePacket<PacketType::TextMessage> {
    static void deserialize(aes256::context& context, ReadBuffer& buffer, TextMessage& packet);
    static void serialize(aes256::context& context, WriteBuffer& buffer, const TextMessage& packet);

    std::uint64_t timestamp; ///< Message timestamp in UTC UNIX milliseconds, UINT64_MAX when server-bound
    std::string username;    ///< Username of the sender, empty when server-bound
    std::string message;     ///< Message text
};

#endif
