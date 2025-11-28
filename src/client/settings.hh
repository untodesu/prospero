// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: settings.hh; Created: Thu Nov 27 2025 00:10:42
// Description: Client settings

#ifndef CLIENT_SETTINGS_HH
#define CLIENT_SETTINGS_HH 1
#pragma once

#include "core/ed25519.hh"

class Settings final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString theme READ theme CONSTANT)
    Q_PROPERTY(QString language READ language CONSTANT)
    Q_PROPERTY(bool hide_on_close READ hide_on_close WRITE set_hide_on_close NOTIFY hide_on_close_changed)
    Q_PROPERTY(bool mute_messages READ mute_messages WRITE set_mute_messages NOTIFY mute_messages_changed)
    Q_PROPERTY(bool mute_mentions READ mute_mentions WRITE set_mute_mentions NOTIFY mute_mentions_changed)
    Q_PROPERTY(QString username READ username WRITE set_username NOTIFY username_changed)
    Q_PROPERTY(QString public_key READ public_key CONSTANT)

public:
    static Settings* instance;

    explicit Settings(QObject* parent = nullptr);
    virtual ~Settings(void) override;

    QString theme(void) const;

    const QString& language(void) const;

    bool hide_on_close(void) const;
    void set_hide_on_close(bool hide);

    bool mute_messages(void) const;
    void set_mute_messages(bool mute);

    bool mute_mentions(void) const;
    void set_mute_mentions(bool mute);

    const QString& username(void) const;
    void set_username(const QString& username);

    const QString& public_key(void) const;
    const ed25519::pkey_buffer& public_key_buffer(void) const;
    const ed25519::skey_buffer& private_key_buffer(void) const;

signals:
    void hide_on_close_changed(void);
    void mute_messages_changed(void);
    void mute_mentions_changed(void);
    void username_changed(void);

private:
    void load_from_config(void);
    void save_to_config(void);

    std::filesystem::path m_config_path;

    QString m_language;

    bool m_hide_on_close;

    bool m_show_timestamps;

    bool m_mute_messages;
    bool m_mute_mentions;

    QString m_username;

    QString m_public_key_string;
    ed25519::pkey_buffer m_public_key;
    ed25519::skey_buffer m_private_key;
};

#endif
