// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: session.cc; Created: Tue Nov 25 2025 17:45:55
// Description: Session management

#include "client/precompiled.hh"

#include "client/session.hh"

#include "core/buffer.hh"
#include "core/exception.hh"
#include "core/protocol.hh"

#include "client/identity.hh"

Session* Session::instance = nullptr;

Session::Session(QObject* parent) : QObject(parent)
{
    m_aes_context = nullptr;

    m_host_timer = new QTimer(this);
    m_host_timer->setInterval(20);
    m_host_timer->setSingleShot(false);
    m_host_timer->start();

    m_host = enet_host_create(nullptr, 1U, PROTOCOL_MAXCHAN, 0U, 0U);

    if(m_host == nullptr) {
        throw core::runtime_error("failed to create a client host");
    }

    connect(m_host_timer, &QTimer::timeout, this, &Session::update_host);

    reset_session_data();
}

Session::~Session(void)
{
    if(m_server) {
        enet_peer_disconnect(m_server, 0U);
        enet_host_flush(m_host);

        while(0 < enet_host_service(m_host, nullptr, 10)) {
            // empty
        }
    }
}

void Session::connect_to_host(const QString& full_address)
{
    quint16 port;
    QLatin1String host;

    auto separator = full_address.indexOf(':');

    if(separator < 0) {
        host = QLatin1String(full_address.toLatin1().constData());
        port = 18137U; // default port
    }
    else {
        host = QLatin1String(full_address.left(separator).toLatin1().constData());
        port = static_cast<quint16>(full_address.mid(separator + 1).toUShort());
    }

    connect_to_host(host, port);
}

void Session::connect_to_host(const QLatin1String& host, quint16 port)
{
    if(m_server) {
        enet_peer_disconnect(m_server, 0U);
        enet_host_flush(m_host);
        reset_session_data();
    }

    ENetAddress address = {};
    enet_address_set_host(&address, host.latin1());
    address.port = port;

    m_server = enet_host_connect(m_host, &address, PROTOCOL_MAXCHAN, 0U);
}

void Session::disconnect_from_host(void)
{
    if(m_server) {
        enet_peer_disconnect(m_server, 0U);
    }
}

void Session::add_system_message(const QString& message)
{
    emit system_message_received(QDateTime::currentDateTime(), message);
}

void Session::send_text_message(const QString& message)
{
    assert(m_server);
    assert(m_aes_context);

    thread_local WriteBuffer buffer;
    thread_local TextMessage packet;

    packet.timestamp = UINT64_MAX; ///< UINT64_MAX when server-bound
    packet.username.clear();       ///< empty when server-bound
    packet.message = message.toStdString();

    buffer.reset();
    buffer.write<std::uint32_t>(TextMessage::ID);
    TextMessage::serialize(m_aes_context, buffer, packet);

    enet_peer_send(m_server, Session::random_channel(), enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

bool Session::is_connected(void) const
{
    return m_server && m_server->state == ENET_PEER_STATE_CONNECTED;
}

bool Session::is_authenticated(void) const
{
    return m_server && m_aes_context;
}

const QString& Session::username(void) const
{
    return m_username;
}

void Session::update_host(void)
{
    assert(m_host);

    thread_local ENetEvent event;

    while(0 < enet_host_service(m_host, &event, 0)) {
        qDebug() << "ENet event type:" << event.type;

        if(event.type == ENET_EVENT_TYPE_CONNECT) {
            emit connection_changed(true);
            continue;
        }

        if(event.type == ENET_EVENT_TYPE_RECEIVE) {
            handle_packet(event.packet, event.channelID);
            enet_packet_destroy(event.packet);
            continue;
        }

        if(event.type == ENET_EVENT_TYPE_DISCONNECT) {
            reset_session_data();
            emit connection_changed(false);
            continue;
        }
    }
}

std::uint32_t Session::random_channel(void)
{
    auto generator = QRandomGenerator::system();

    return generator->bounded(0U, PROTOCOL_MAXCHAN - 1U);
}

void Session::reset_session_data(void)
{
    if(m_aes_context) {
        aes256::destroy(m_aes_context);
    }

    m_server = nullptr;
    m_aes_context = nullptr;
    m_username.clear();
}

void Session::handle_packet(const ENetPacket* packet, quint32 channel)
{
    assert(packet);
    assert(channel < PROTOCOL_MAXCHAN);

    thread_local ReadBuffer buffer;
    thread_local AuthChallengeRequest auth_request;
    thread_local AuthChallengeResult auth_result;
    thread_local SystemMessage system_message;
    thread_local TextMessage text_message;

    buffer.reset(packet->data, packet->dataLength);

    auto packet_type = buffer.read<std::uint32_t>();

    if(packet_type == AuthChallengeRequest::ID) {
        if(channel == PROTOCOL_AUTHCHAN && m_aes_context == nullptr) {
            AuthChallengeRequest::deserialize(buffer, auth_request);
            handle_auth_challenge_request(auth_request);
            return;
        }

        enet_peer_disconnect(m_server, 0U);
        return;
    }

    if(packet_type == AuthChallengeResult::ID) {
        if(channel == PROTOCOL_AUTHCHAN && m_aes_context == nullptr) {
            AuthChallengeResult::deserialize(buffer, auth_result);
            handle_auth_challenge_result(auth_result);
            return;
        }

        enet_peer_disconnect(m_server, 0U);
        return;
    }

    if(m_aes_context) {
        switch(packet_type) {
            case SystemMessage::ID:
                SystemMessage::deserialize(m_aes_context, buffer, system_message);
                handle_system_message(system_message);
                break;

            case TextMessage::ID:
                TextMessage::deserialize(m_aes_context, buffer, text_message);
                handle_text_message(text_message);
                break;

            default:
                enet_peer_disconnect(m_server, 0U);
                break;
        }
    }
}

void Session::handle_auth_challenge_request(const AuthChallengeRequest& packet)
{
    assert(m_server);
    assert(m_aes_context == nullptr);

    auto& public_key = Identity::instance->public_key();
    auto& private_key = Identity::instance->private_key();

    thread_local WriteBuffer buffer;
    thread_local AuthChallengeResponse response;

    response.client_pkey = public_key;
    response.username = std::string("prosperoclient"); // TODO: make configurable
    ed25519::sign(public_key, private_key, packet.challenge_data, response.signature);

    buffer.reset();
    buffer.write<std::uint32_t>(AuthChallengeResponse::ID);
    AuthChallengeResponse::serialize(buffer, response);

    enet_peer_send(m_server, PROTOCOL_AUTHCHAN, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Session::handle_auth_challenge_result(const AuthChallengeResult& packet)
{
    assert(m_server);
    assert(m_aes_context == nullptr);

    if(packet.status == AuthChallengeResult::E_OK) {
        ed25519::exch_buffer shared_secret;
        ed25519::generate_exch(Identity::instance->private_key(), packet.server_pkey, shared_secret);
        aes256::create(m_aes_context, shared_secret);

        m_username = QString::fromStdString(packet.username);
    }
    else {
        QString message;

        switch(packet.status) {
            case AuthChallengeResult::E_CRED:
                message = tr("invalid credentials");
                break;

            case AuthChallengeResult::E_TIME:
                message = tr("authentication timeout");
                break;

            case AuthChallengeResult::E_UNREC:
                message = tr("unrecognized public key");
                break;

            default:
                message = tr("unknown error");
                break;
        }

        add_system_message(tr("Auth failed: %1").arg(message));
    }
}

void Session::handle_system_message(const SystemMessage& packet)
{
    assert(m_aes_context);

    auto timestamp = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(packet.timestamp), QTimeZone::UTC);
    auto message = QString::fromStdString(packet.message);

    emit system_message_received(timestamp, message);
}

void Session::handle_text_message(const TextMessage& packet)
{
    assert(m_aes_context);

    auto timestamp = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(packet.timestamp), QTimeZone::UTC);
    auto sender = QString::fromStdString(packet.username);
    auto message = QString::fromStdString(packet.message);

    emit text_message_received(timestamp, sender, message);
}
