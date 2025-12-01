// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: session.cc; Created: Tue Nov 25 2025 17:45:55
// Description: Session management

#include "client/precompiled.hh"

#include "client/session.hh"

#include "core/buffer.hh"
#include "core/exception.hh"
#include "core/protocol.hh"
#include "core/version.hh"

#include "client/settings.hh"

Session* Session::instance = nullptr;

Session::Session(QObject* parent) : QObject(parent)
{
    m_host_timer = new QTimer(this);
    m_host_timer->setInterval(20);
    m_host_timer->setSingleShot(false);
    m_host_timer->start();

    m_host = enet_host_create(nullptr, 1U, 1U, 0U, 0U);

    if(m_host == nullptr) {
        throw core::runtime_error("failed to create a client host");
    }

    m_server = nullptr;
    m_aes_context = nullptr;

    connect(m_host_timer, &QTimer::timeout, this, &Session::update_host);

    reset_session_data();

    connect(this, &Session::address_resolved, this, &Session::perform_connection);
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

bool Session::is_connected(void) const
{
    return m_server && m_server->state == ENET_PEER_STATE_CONNECTED;
}

bool Session::is_authenticated(void) const
{
    return m_server && m_server->state == ENET_PEER_STATE_CONNECTED && m_aes_context;
}

const QString& Session::assigned_username(void) const
{
    return m_assigned_username;
}

void Session::connect_to_host(const QString& full_address)
{
    quint16 port;
    QString host;

    auto separator = full_address.indexOf(':');

    if(separator < 0) {
        host = full_address;
        port = 18137U; // default port
    }
    else {
        host = full_address.left(separator);
        port = static_cast<quint16>(full_address.mid(separator + 1).toUShort());
    }

    connect_to_host(host.toStdString(), port);
}

void Session::connect_to_host(const std::string& host, quint16 port)
{
    if(m_server) {
        enet_peer_disconnect(m_server, 0U);
        enet_host_flush(m_host);
        reset_session_data();
    }

    emit connection_started();

    auto host_copy = std::string(host);
    auto port_copy = port;

    auto future = QtConcurrent::run([this, host_copy, port_copy] {
        ENetAddress address;
        enet_address_set_host(&address, host_copy.c_str());
        address.port = port_copy;

        emit address_resolved(address);
    });
}

void Session::disconnect_from_host(void)
{
    if(m_server) {
        enet_peer_disconnect(m_server, 0U);
    }
}

void Session::add_notification_peer_join(const QDateTime& timestamp, const QString& username)
{
    emit notification_received(timestamp, tr("%1 connected to the server").arg(username));
}

void Session::add_notification_peer_left(const QDateTime& timestamp, const QString& username)
{
    emit notification_received(timestamp, tr("%1 disconnected from the server").arg(username));
}

void Session::add_notification_text_mesg(const QDateTime& timestamp, const QString& message)
{
    emit notification_received(timestamp, message);
}

void Session::add_notification_user_away(const QDateTime& timestamp, const QString& username, const QString& away_message)
{
    emit notification_received(timestamp, tr("%1 is now away: %2").arg(username).arg(away_message));
}

void Session::add_notification_user_back(const QDateTime& timestamp, const QString& username)
{
    emit notification_received(timestamp, tr("%1 is no longer away").arg(username));
}

void Session::add_notification_modr_kick(const QDateTime& timestamp, const QString& username)
{
    emit notification_received(timestamp, tr("%1 has been kicked").arg(username));
}

void Session::send_text_message(const QString& message)
{
    assert(m_server);
    assert(m_aes_context);

    thread_local WriteBuffer buffer;
    thread_local BasicMessage packet;

    packet.timestamp = UINT64_MAX; ///< UINT64_MAX when server-bound
    packet.username.clear();       ///< empty when server-bound
    packet.message = message.toStdString();

    buffer.reset();
    buffer.write<std::uint32_t>(BasicMessage::ID);
    BasicMessage::serialize(m_aes_context, buffer, packet);

    enet_peer_send(m_server, 0U, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Session::perform_connection(ENetAddress address)
{
    m_server = enet_host_connect(m_host, &address, 1U, 0U);

    thread_local ENetEvent event;

    if(m_server) {
        for(int i = 0; i < 150; ++i) {
            if(0 < enet_host_service(m_host, &event, 10U)) {
                if(event.type == ENET_EVENT_TYPE_CONNECT) {
                    emit connection_changed();
                    return;
                }
            }

            QCoreApplication::processEvents();
        }
    }

    enet_peer_reset(m_server);
    reset_session_data();

    emit notification_received(QDateTime::currentDateTime(), tr("Failed to connect to the server"));
    emit connection_changed();
}

void Session::update_host(void)
{
    assert(m_host);

    thread_local ENetEvent event;

    while(0 < enet_host_service(m_host, &event, 0)) {
        if(event.type == ENET_EVENT_TYPE_RECEIVE) {
            handle_packet(event.packet);
            enet_packet_destroy(event.packet);
            continue;
        }

        if(event.type == ENET_EVENT_TYPE_DISCONNECT) {
            add_notification_text_mesg(QDateTime::currentDateTime(), tr("Disconnected from server"));
            reset_session_data();
            emit connection_changed();
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
    m_assigned_username.clear();
}

void Session::handle_packet(const ENetPacket* packet)
{
    assert(packet);

    thread_local ReadBuffer buffer;
    thread_local AuthRequest auth_request;
    thread_local AuthResult auth_result;
    thread_local Notification notification;
    thread_local BasicMessage text_message;

    buffer.reset(packet->data, packet->dataLength);

    auto packet_type = buffer.read<std::uint32_t>();

    if(packet_type == AuthRequest::ID) {
        if(m_aes_context == nullptr) {
            AuthRequest::deserialize(buffer, auth_request);
            handle_auth_request(auth_request);
        }
        else {
            enet_peer_disconnect(m_server, 0U);
        }

        return;
    }

    if(packet_type == AuthResult::ID) {
        if(m_aes_context == nullptr) {
            AuthResult::deserialize(buffer, auth_result);
            handle_auth_result(auth_result);
        }
        else {
            enet_peer_disconnect(m_server, 0U);
        }

        return;
    }

    if(m_aes_context) {
        switch(packet_type) {
            case Notification::ID:
                Notification::deserialize(m_aes_context, buffer, notification);
                handle_notification(notification);
                break;

            case BasicMessage::ID:
                BasicMessage::deserialize(m_aes_context, buffer, text_message);
                handle_basic_message(text_message);
                break;

            default:
                enet_peer_disconnect(m_server, 0U);
                break;
        }
    }
}

void Session::handle_auth_request(const AuthRequest& packet)
{
    assert(m_server);
    assert(m_aes_context == nullptr);

    auto client_version_string = QStringLiteral("%1.%2.%3").arg(version::major).arg(version::minor).arg(version::patch);
    auto server_version_string = QStringLiteral("%1.%2.%3").arg(packet.version_major).arg(packet.version_minor).arg(packet.version_patch);

    if(version::major < packet.version_major) {
        auto message = tr("Outdated client! Server runs on %1").arg(server_version_string);
        add_notification_text_mesg(QDateTime::currentDateTime(), message);
        enet_peer_disconnect(m_server, 0U);
        return;
    }

    if(version::major > packet.version_major) {
        auto message = tr("Outdated server! Server runs on %1").arg(client_version_string);
        add_notification_text_mesg(QDateTime::currentDateTime(), message);
        enet_peer_disconnect(m_server, 0U);
        return;
    }

    auto version_full_compatible = true;
    version_full_compatible = version_full_compatible && version::minor == packet.version_minor;
    version_full_compatible = version_full_compatible && version::patch == packet.version_patch;

    if(!version_full_compatible) {
        auto message = tr("Potentially incompatible versions [SV %1 // CL %2]").arg(server_version_string).arg(client_version_string);
        add_notification_text_mesg(QDateTime::currentDateTime(), message);
    }

    auto& public_key = Settings::instance->public_key_buffer();
    auto& private_key = Settings::instance->private_key_buffer();

    thread_local WriteBuffer buffer;
    thread_local AuthResponse response;

    response.public_key = public_key;
    response.desired_username = Settings::instance->username().toStdString();
    ed25519::sign(public_key, private_key, packet.challenge, response.signature);

    buffer.reset();
    buffer.write<std::uint32_t>(AuthResponse::ID);
    AuthResponse::serialize(buffer, response);

    enet_peer_send(m_server, 0U, enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE));
}

void Session::handle_auth_result(const AuthResult& packet)
{
    assert(m_server);
    assert(m_aes_context == nullptr);

    ed25519::exch_buffer shared_secret;
    ed25519::generate_exch(Settings::instance->private_key_buffer(), packet.public_key, shared_secret);
    aes256::create(m_aes_context, shared_secret);

    m_assigned_username = QString::fromStdString(packet.assigned_username);

    emit connection_changed();
}

void Session::handle_notification(const Notification& packet)
{
    assert(m_aes_context);

    auto timestamp = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(packet.timestamp), QTimeZone::UTC);
    auto text_1 = QString::fromStdString(packet.text_1);
    auto text_2 = QString::fromStdString(packet.text_2);

    switch(packet.type) {
        case Notification::T_PEER_JOIN:
            add_notification_peer_join(timestamp, text_1);
            break;

        case Notification::T_PEER_LEFT:
            add_notification_peer_left(timestamp, text_1);
            break;

        case Notification::T_TEXT_MESG:
            add_notification_text_mesg(timestamp, text_1);
            break;

        case Notification::T_USER_AWAY:
            add_notification_user_away(timestamp, text_1, text_2);
            break;

        case Notification::T_USER_BACK:
            add_notification_user_back(timestamp, text_1);
            break;

        case Notification::T_MODR_KICK:
            add_notification_modr_kick(timestamp, text_1);
            break;

        default:
            qDebug() << "Received unknown notification type:" << packet.type;
            return;
    }
}

void Session::handle_basic_message(const BasicMessage& packet)
{
    assert(m_aes_context);

    auto timestamp = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(packet.timestamp), QTimeZone::UTC);
    auto username = QString::fromStdString(packet.username.substr(0U, BasicMessage::MAX_USERNAME_LENGTH));
    auto message = QString::fromStdString(packet.message.substr(0U, BasicMessage::MAX_MESSAGE_LENGTH));

    if(message.startsWith("data:")) {
        auto mimetype_end = message.indexOf(',');

        if(mimetype_end >= 0) {
            constexpr std::size_t header_end = 5U;

            auto mimetype = message.mid(header_end, mimetype_end - header_end);

            if(mimetype.startsWith("image/")) {
                emit image_message_received(timestamp, username, message);
                return;
            }
        }
    }

    emit text_message_received(timestamp, username, message);
}
