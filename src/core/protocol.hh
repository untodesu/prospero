// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: protocol.hh; Created: Mon Nov 24 2025 23:34:49
// Description: Packet definitions

#ifndef CORE_PROTOCOL_HH
#define CORE_PROTOCOL_HH 1
#pragma once

#include "core/aes256.hh"
#include "core/ed25519.hh"

class ReadBuffer;
class WriteBuffer;

enum class PacketType : std::uint32_t {
    AuthRequest = 1U,
    AuthResponse,
    AuthResult,
    Notification,
    TextMessage,
};

template<PacketType Type>
struct BasePacket {
    constexpr static std::uint32_t ID = static_cast<std::uint32_t>(Type);
};

struct AuthRequest final : public BasePacket<PacketType::AuthRequest> {
    static void deserialize(ReadBuffer& buffer, AuthRequest& packet);
    static void serialize(WriteBuffer& buffer, const AuthRequest& packet);

    std::uint32_t version_major;
    std::uint32_t version_minor;
    std::uint32_t version_patch;
    std::uint64_t auth_timestamp;
    ed25519::seed_buffer challenge;
};

struct AuthResponse final : public BasePacket<PacketType::AuthResponse> {
    static void deserialize(ReadBuffer& buffer, AuthResponse& packet);
    static void serialize(WriteBuffer& buffer, const AuthResponse& packet);

    ed25519::pkey_buffer public_key {};
    ed25519::sign_buffer signature {};
    std::string desired_username;
};

struct AuthResult final : public BasePacket<PacketType::AuthResult> {
    static void deserialize(ReadBuffer& buffer, AuthResult& packet);
    static void serialize(WriteBuffer& buffer, const AuthResult& packet);

    ed25519::pkey_buffer public_key {};
    std::string assigned_username;
};

struct Notification final : public BasePacket<PacketType::Notification> {
    constexpr static std::uint32_t T_PEER_JOIN = 0x00000001U; ///< User <text> has connected
    constexpr static std::uint32_t T_PEER_LEFT = 0x00000002U; ///< User <text> has disconnected
    constexpr static std::uint32_t T_TEXT_MESG = 0x00000003U; ///< Generic text message
    constexpr static std::uint32_t T_USER_AWAY = 0x00000004U; ///< User <text> is now away
    constexpr static std::uint32_t T_USER_BACK = 0x00000005U; ///< User <text> is no longer away
    constexpr static std::uint32_t T_MODR_KICK = 0x00000006U; ///< User <text> has been kicked

    static void deserialize(aes256::context& context, ReadBuffer& buffer, Notification& packet);
    static void serialize(aes256::context& context, WriteBuffer& buffer, const Notification& packet);

    std::uint64_t timestamp {};
    std::uint32_t type {};
    std::string text_1;
    std::string text_2;
};

struct TextMessage final : public BasePacket<PacketType::TextMessage> {
    constexpr static std::size_t MAX_USERNAME_LENGTH = 96U;
    constexpr static std::size_t MAX_MESSAGE_LENGTH = 512U;

    static void deserialize(aes256::context& context, ReadBuffer& buffer, TextMessage& packet);
    static void serialize(aes256::context& context, WriteBuffer& buffer, const TextMessage& packet);

    std::uint64_t timestamp {};
    std::string username;
    std::string message;
};

#endif
