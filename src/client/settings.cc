// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: settings.cc; Created: Thu Nov 27 2025 00:15:52
// Description: Client settings

#include "client/precompiled.hh"

#include "client/settings.hh"

#include "core/config.hh"

Settings* Settings::instance = nullptr;

Settings::Settings(QObject* parent) : QObject(parent)
{
    std::filesystem::path app_data_directory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString());
    std::filesystem::create_directories(app_data_directory);

    m_config_path = app_data_directory / "client.conf";

    load_from_config();
}

Settings::~Settings(void)
{
    save_to_config();
}

const QString& Settings::language(void) const
{
    return m_language;
}

QString Settings::theme(void) const
{
    return QQuickStyle::name();
}

bool Settings::hide_on_close(void) const
{
    return m_hide_on_close;
}

void Settings::set_hide_on_close(bool hide)
{
    if(m_hide_on_close != hide) {
        m_hide_on_close = hide;

        emit hide_on_close_changed();

        save_to_config();
    }
}

bool Settings::mute_messages(void) const
{
    return m_mute_messages;
}

void Settings::set_mute_messages(bool mute)
{
    if(m_mute_messages != mute) {
        m_mute_messages = mute;

        emit mute_messages_changed();

        save_to_config();
    }
}

bool Settings::mute_mentions(void) const
{
    return m_mute_mentions;
}

void Settings::set_mute_mentions(bool mute)
{
    if(m_mute_mentions != mute) {
        m_mute_mentions = mute;

        emit mute_mentions_changed();

        save_to_config();
    }
}

const QString& Settings::username(void) const
{
    return m_username;
}

void Settings::set_username(const QString& username)
{
    if(m_username.compare(username)) {
        m_username = username;

        emit username_changed();

        save_to_config();
    }
}

const QString& Settings::public_key(void) const
{
    return m_public_key_string;
}

const ed25519::pkey_buffer& Settings::public_key_buffer(void) const
{
    return m_public_key;
}

const ed25519::skey_buffer& Settings::private_key_buffer(void) const
{
    return m_private_key;
}

void Settings::load_from_config(void)
{
    Config config(m_config_path);

    QString default_username = qgetenv("USER");

    if(default_username.isEmpty()) {
        default_username = qgetenv("USERNAME");
    }

    if(default_username.isEmpty()) {
        default_username = "prosperoclient";
    }

    QQuickStyle::setStyle(QString::fromStdString(std::string(config.value<std::string_view>("theme", "Fusion"))));

    m_language = QString::fromStdString(std::string(config.value<std::string_view>("language", std::string_view())));

    m_hide_on_close = config.value<bool>("hide_on_close", true);

    m_mute_messages = config.value<bool>("mute_messages", false);
    m_mute_mentions = config.value<bool>("mute_mentions", false);

    m_username = QString::fromStdString(std::string(config.value<std::string_view>("username", default_username.toStdString())));

    auto public_key_hexstring = config.value<std::string_view>("public_key");
    auto private_key_hexstring = config.value<std::string_view>("private_key");

    auto keypair_valid = true;
    keypair_valid = keypair_valid && ed25519::import_public_key(public_key_hexstring, m_public_key);
    keypair_valid = keypair_valid && ed25519::import_private_key(private_key_hexstring, m_private_key);

    if(!keypair_valid) {
        ed25519::seed_buffer seed;
        ed25519::generate_seed(seed);
        ed25519::generate_keys(seed, m_public_key, m_private_key);
    }

    m_public_key_string = QString::fromStdString(ed25519::export_public_key(m_public_key));
}

void Settings::save_to_config(void)
{
    Config config;

    config.set_value<std::string_view>("theme", QQuickStyle::name().toStdString());

    if(m_language.size()) {
        // Only save the language value if it's not empty; this is done
        // to have Qt first try to use the system locale by default, and
        // if the user _needs_ to force a specific language, they can set it in the config
        config.set_value<std::string_view>("language", m_language.toStdString());
    }

    config.set_value<bool>("hide_on_close", m_hide_on_close);

    config.set_value<bool>("mute_messages", m_mute_messages);
    config.set_value<bool>("mute_mentions", m_mute_mentions);

    config.set_value<std::string_view>("username", m_username.toStdString());

    config.set_value<std::string_view>("public_key", ed25519::export_public_key(m_public_key));
    config.set_value<std::string_view>("private_key", ed25519::export_private_key(m_private_key));

    config.write(m_config_path);
}
