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
    Q_PROPERTY(QString assigned_username READ assigned_username NOTIFY connection_changed)
    Q_PROPERTY(QString desired_username READ desired_username WRITE set_desired_username NOTIFY desired_username_changed)

public:
    static Session* instance;

    explicit Session(QObject* parent = nullptr);
    virtual ~Session(void) override;

    bool is_connected(void) const;
    const QString& assigned_username(void) const;
    const QString& desired_username(void) const;
    void set_desired_username(const QString& username);

public:
    Q_INVOKABLE void connect_to_host(const QString& full_address);
    Q_INVOKABLE void connect_to_host(const QLatin1String& host, quint16 port);
    Q_INVOKABLE void disconnect_from_host(void);

    Q_INVOKABLE void add_notification_user_join(const QDateTime& timestamp, const QString& username);
    Q_INVOKABLE void add_notification_user_left(const QDateTime& timestamp, const QString& username);
    Q_INVOKABLE void add_notification_permissions_modified(const QDateTime& timestamp);
    Q_INVOKABLE void add_notification_generic(const QDateTime& timestamp, const QString& message);

    Q_INVOKABLE void send_text_message(const QString& message);

    Q_INVOKABLE void add_system_message(const QString& message);
    Q_INVOKABLE void send_text_message(const QString& message);

    Q_INVOKABLE const QString& desired_username(void) const;
    Q_INVOKABLE void set_desired_username(const QString& username);

    Q_INVOKABLE bool is_connected(void) const;
    Q_INVOKABLE bool is_authenticated(void) const;
    Q_INVOKABLE const QString& username(void) const;

signals:
    void desired_username_changed(void);
    void connection_changed(bool is_connected);
    void system_message_received(const QDateTime& timetamp, const QString& message);
    void text_message_received(const QDateTime& timetamp, const QString& sender, const QString& message);

private slots:
    void update_host(void);

private:
    static std::uint32_t random_channel(void);

    void load_from_config(void);
    void save_to_config(void);

    void reset_session_data(void);

    void handle_packet(const ENetPacket* packet, quint32 channel);

    void handle_auth_challenge_request(const AuthChallengeRequest& packet);
    void handle_auth_challenge_result(const AuthChallengeResult& packet);
    void handle_system_message(const SystemMessage& packet);
    void handle_text_message(const TextMessage& packet);

    std::filesystem::path m_config_path;

    QString m_desired_username;

    ENetHost* m_host;
    QTimer* m_host_timer;

    ENetPeer* m_server { nullptr };
    aes256::context m_aes_context { nullptr };
    QString m_username;

    QRandomGenerator m_random;
};

#endif
