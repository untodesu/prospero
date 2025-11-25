// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: session.cc; Created: Tue Nov 25 2025 17:45:55
// Description: Session management

#include "client/precompiled.hh"

#include "client/session.hh"

#include "core/buffer.hh"
#include "core/exception.hh"
#include "core/protocol.hh"

Session::Session(QObject* parent) : QObject(parent)
{
    m_host_timer = new QTimer(this);
    m_host_timer->setInterval(20);
    m_host_timer->setSingleShot(false);
    m_host_timer->start();

    m_host = enet_host_create(nullptr, 1U, PROTOCOL_MAXCHAN, 0U, 0U);

    if(m_host == nullptr) {
        throw core::runtime_error("failed to create a client host");
    }

    connect(m_host_timer, &QTimer::timeout, this, &Session::update_host);

    m_channels.resize(PROTOCOL_MAXCHAN, QString());

    reset_session_data();
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

void Session::send_text_message(quint32 channel, const QString& message)
{
    assert(m_server);
    assert(m_aes_context);
    assert(channel < PROTOCOL_MAXCHAN);

    thread_local WriteBuffer buffer;
    thread_local TextMessage packet;

    packet.timestamp = UINT64_MAX; ///< UINT64_MAX when server-bound
    packet.username.clear();       ///< empty when server-bound
    packet.message = message.toStdString();

    buffer.reset();
    buffer.write<std::uint32_t>(TextMessage::ID);
    TextMessage::serialize(m_aes_context, buffer, packet);

    enet_peer_send(m_server, channel, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
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

const QList<QString>& Session::channels(void) const
{
    return m_channels;
}

void Session::update_host(void)
{
    assert(m_host);

    thread_local ENetEvent event;

    while(0 < enet_host_service(m_host, &event, 0)) {
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

void Session::reset_session_data(void)
{
    if(m_aes_context) {
        aes256::destroy(m_aes_context);
    }

    m_server = nullptr;
    m_aes_context = nullptr;
    m_username.clear();
    m_channels.fill(QString());
}

void Session::handle_packet(const ENetPacket* packet, quint32 channel)
{
}

void Session::handle_auth_challenge_request(const AuthChallengeRequest& request)
{
}

void Session::handle_auth_challenge_result(const AuthChallengeResult& result)
{
    assert(m_server);
    assert(m_aes_context);

    if(result.status == AuthChallengeResult::E_OK) {
    }
}
