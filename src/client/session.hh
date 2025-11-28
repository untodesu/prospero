// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: session.hh; Created: Tue Nov 25 2025 17:38:13
// Description: Session management

#ifndef CLIENT_SESSION_HH
#define CLIENT_SESSION_HH 1
#pragma once

#include "core/aes256.hh"

struct AuthRequest;
struct AuthResult;
struct Notification;
struct TextMessage;

class Session final : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool is_connected READ is_connected NOTIFY connection_changed)
    Q_PROPERTY(bool is_authenticated READ is_authenticated NOTIFY connection_changed)
    Q_PROPERTY(QString assigned_username READ assigned_username NOTIFY connection_changed)

public:
    static Session* instance;

    explicit Session(QObject* parent = nullptr);
    virtual ~Session(void) override;

    bool is_connected(void) const;
    bool is_authenticated(void) const;

    const QString& assigned_username(void) const;

public:
    Q_INVOKABLE void connect_to_host(const QString& full_address);
    Q_INVOKABLE void connect_to_host(const std::string& host, quint16 port);
    Q_INVOKABLE void disconnect_from_host(void);

    Q_INVOKABLE void add_notification_peer_join(const QDateTime& timestamp, const QString& username);
    Q_INVOKABLE void add_notification_peer_left(const QDateTime& timestamp, const QString& username);
    Q_INVOKABLE void add_notification_text_mesg(const QDateTime& timestamp, const QString& message);
    Q_INVOKABLE void add_notification_user_away(const QDateTime& timestamp, const QString& username, const QString& away_message);
    Q_INVOKABLE void add_notification_user_back(const QDateTime& timestamp, const QString& username);
    Q_INVOKABLE void add_notification_modr_kick(const QDateTime& timestamp, const QString& username);

    Q_INVOKABLE void send_text_message(const QString& message);

signals:
    void connection_changed(void);
    void connection_started(void);
    void notification_received(const QDateTime& timestamp, const QString& message);
    void text_message_received(const QDateTime& timestamp, const QString& username, const QString& message);

    void address_resolved(ENetAddress address);

private slots:
    void perform_connection(ENetAddress address);
    void update_host(void);

private:
    void reset_session_data(void);

    void handle_packet(const ENetPacket* packet);

    void handle_auth_request(const AuthRequest& packet);
    void handle_auth_result(const AuthResult& packet);
    void handle_notification(const Notification& packet);
    void handle_text_message(const TextMessage& packet);

    ENetHost* m_host;
    QTimer* m_host_timer;

    ENetPeer* m_server;
    aes256::context m_aes_context;
    QString m_assigned_username;
};

#endif
