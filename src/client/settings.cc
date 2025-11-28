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

bool Settings::minimize_on_close(void) const
{
    return m_minimize_on_close;
}

void Settings::set_minimize_on_close(bool minimize)
{
    if(m_minimize_on_close != minimize) {
        m_minimize_on_close = minimize;

        emit minimize_on_close_changed();

        save_to_config();
    }
}

bool Settings::show_timestamps(void) const
{
    return m_show_timestamps;
}

void Settings::set_show_timestamps(bool show)
{
    if(m_show_timestamps != show) {
        m_show_timestamps = show;

        emit show_timestamps_changed();

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

    m_minimize_on_close = config.value<bool>("minimize_on_close", true);

    m_show_timestamps = config.value<bool>("show_timestamps", true);

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

    config.set_value<bool>("minimize_on_close", m_minimize_on_close);

    config.set_value<bool>("show_timestamps", m_show_timestamps);

    config.set_value<bool>("mute_messages", m_mute_messages);
    config.set_value<bool>("mute_mentions", m_mute_mentions);

    config.set_value<std::string_view>("username", m_username.toStdString());

    config.set_value<std::string_view>("public_key", ed25519::export_public_key(m_public_key));
    config.set_value<std::string_view>("private_key", ed25519::export_private_key(m_private_key));

    config.write(m_config_path);
}
