// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: protocol.hh; Created: Mon Nov 24 2025 23:34:49
// Description: Packet definitions

#ifndef CORE_PROTOCOL_HH
#define CORE_PROTOCOL_HH 1
#pragma once

#include "core/aes256.hh"
#include "core/ed25519.hh"

namespace core
{
class ReadBuffer;
class WriteBuffer;
} // namespace core

namespace core
{
enum class PacketType : std::uint32_t {
    AuthChallengeRequest = 1U, ///< Sent to the client as soon as it connects
    AuthChallengeResponse,     ///< Sent to the server in response to AuthChallengeRequest
    AuthChallengeResult,       ///< Sent to the client as a response to AuthChallengeResponse
    ChannelDefinition,         ///< Sent after AuthChallengeResult to define channels on the server
    ChannelUserJoin,           ///< Sent when a user joins (or requests to join) a channel
    ChannelUserLeave,          ///< Sent when a user leaves (or requests to leave) a channel
    SystemMessage,             ///< Plain-text server-wide system message
    TextMessage,               ///< Plain-text message sent to a channel
};
} // namespace core

namespace core
{
constexpr static std::uint32_t AUTH_OK = 0x00000000U;    ///< Authentication successful
constexpr static std::uint32_t AUTH_CRED = 0x00000001U;  ///< Invalid credentials
constexpr static std::uint32_t AUTH_TIME = 0x00000002U;  ///< Authentication timed out
constexpr static std::uint32_t AUTH_UNREC = 0x00000003U; ///< Unrecognized client signature
} // namespace core

namespace core
{
constexpr static std::uint32_t SEVR_INFORM = 0x00000001U;   ///< Informational system message
constexpr static std::uint32_t SEVR_WARNING = 0x00000002U;  ///< Warning system message
constexpr static std::uint32_t SEVR_CRITICAL = 0x00000003U; ///< Critical system message
} // namespace core

namespace core
{
struct AuthChallengeRequest final {
    CORE_API static void deserialize(ReadBuffer& buffer, AuthChallengeRequest& packet);
    CORE_API static void serialize(WriteBuffer& buffer, const AuthChallengeRequest& packet);

    core::ed25519::pkey_buffer server_pkey;    ///< Server's ed25519 public key
    core::ed25519::seed_buffer challenge_data; ///< Random nonce to be signed by the client
    std::uint64_t challenge_timestamp;         ///< Challenge timestamp in UTC UNIX milliseconds
};

struct AuthChallengeResponse final {
    CORE_API static void deserialize(ReadBuffer& buffer, AuthChallengeResponse& packet);
    CORE_API static void serialize(WriteBuffer& buffer, const AuthChallengeResponse& packet);

    core::ed25519::pkey_buffer client_pkey; ///< Client's ed25519 public key
    core::ed25519::sign_buffer signature;   ///< Signature of the challenge_data from AuthChallengeRequest
    std::string desired_username;           ///< Desired username for the session
};

struct AuthChallengeResult final {
    CORE_API static void deserialize(ReadBuffer& buffer, AuthChallengeResult& packet);
    CORE_API static void serialize(WriteBuffer& buffer, const AuthChallengeResult& packet);

    std::uint32_t auth_status;      ///< One of the core::AUTH_* constants
    std::uint64_t assigned_user_id; ///< Assigned user ID on AUTH_OK, UINT64_MAX otherwise
    std::string assigned_username;  ///< Assigned username on AUTH_OK, empty otherwise
};

struct ChannelDefinition final {
    CORE_API static void deserialize(core::aes256::context& context, ReadBuffer& buffer, ChannelDefinition& packet);
    CORE_API static void serialize(core::aes256::context& context, WriteBuffer& buffer, const ChannelDefinition& packet);

    std::uint64_t channel_id; ///< Unique channel ID
    std::string channel_name; ///< Channel name
};

struct ChannelUserJoin final {
    CORE_API static void deserialize(core::aes256::context& context, ReadBuffer& buffer, ChannelUserJoin& packet);
    CORE_API static void serialize(core::aes256::context& context, WriteBuffer& buffer, const ChannelUserJoin& packet);

    std::uint64_t user_id;    ///< User ID of the joining user, UINT64_MAX when server-bound
    std::uint64_t channel_id; ///< Channel ID to join
    std::string username;     ///< Username of the joining user, empty when server-bound
};

struct ChannelUserLeave final {
    CORE_API static void deserialize(core::aes256::context& context, ReadBuffer& buffer, ChannelUserLeave& packet);
    CORE_API static void serialize(core::aes256::context& context, WriteBuffer& buffer, const ChannelUserLeave& packet);

    std::uint64_t user_id;    ///< User ID of the leaving user, UINT64_MAX when server-bound
    std::uint64_t channel_id; ///< Channel ID to leave
};

struct SystemMessage final {
    CORE_API static void deserialize(core::aes256::context& context, ReadBuffer& buffer, SystemMessage& packet);
    CORE_API static void serialize(core::aes256::context& context, WriteBuffer& buffer, const SystemMessage& packet);

    std::uint32_t severity;  ///< One of the core::SEVR_* constants
    std::uint64_t timestamp; ///< Message timestamp in UTC UNIX milliseconds
    std::string message;     ///< System message text
};

struct TextMessage final {
    CORE_API static void deserialize(core::aes256::context& context, ReadBuffer& buffer, TextMessage& packet);
    CORE_API static void serialize(core::aes256::context& context, WriteBuffer& buffer, const TextMessage& packet);

    std::uint64_t channel_id; ///< Channel ID to which the message is sent
    std::uint64_t user_id;    ///< User ID of the sender, UINT64_MAX when server-bound
    std::uint64_t timestamp;  ///< Message timestamp in UTC UNIX milliseconds, UINT64_MAX when server-bound
    std::string message;      ///< Message text
};
} // namespace core

#endif // CORE_PROTOCOL_HH
