// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: session.hh; Created: Tue Nov 25 2025 17:38:13
// Description: Session management

#ifndef CLIENT_SESSION_HH
#define CLIENT_SESSION_HH 1
#pragma once

#include "core/aes256.hh"

struct AuthChallengeRequest;
struct AuthChallengeResult;

class Session final : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool is_connected READ is_connected NOTIFY connection_changed)
    Q_PROPERTY(bool is_authenticated READ is_authenticated NOTIFY authentication_changed)
    Q_PROPERTY(QString username READ username NOTIFY authentication_changed)
    Q_PROPERTY(QList<QString> channels READ channels NOTIFY channels_changed)

public:
    explicit Session(QObject* parent = nullptr);
    virtual ~Session(void) override = default;

    Q_INVOKABLE void connect_to_host(const QString& full_address);
    Q_INVOKABLE void connect_to_host(const QLatin1String& host, quint16 port);
    Q_INVOKABLE void disconnect_from_host(void);

    Q_INVOKABLE void send_text_message(quint32 channel, const QString& message);

    Q_INVOKABLE bool is_connected(void) const;
    Q_INVOKABLE bool is_authenticated(void) const;
    Q_INVOKABLE const QString& username(void) const;
    Q_INVOKABLE const QList<QString>& channels(void) const;

signals:
    void connection_changed(bool is_connected);
    void authentication_changed(bool is_authenticated, quint32 status);
    void channels_changed(void);

    void system_message_received(quint32 channel, const QDateTime& timetamp, const QString& message);
    void text_message_received(quint32 channel, const QDateTime& timetamp, const QString& sender, const QString& message);

private slots:
    void update_host(void);

private:
    void reset_session_data(void);
    void handle_packet(const ENetPacket* packet, quint32 channel);
    void handle_auth_challenge_request(const AuthChallengeRequest& request);
    void handle_auth_challenge_result(const AuthChallengeResult& result);

    ENetHost* m_host;
    QTimer* m_host_timer;

    ENetPeer* m_server { nullptr };
    aes256::context m_aes_context { nullptr };
    QString m_username;

    QList<QString> m_channels;
};

#endif
