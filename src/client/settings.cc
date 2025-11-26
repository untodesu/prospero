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

    m_username = QString::fromStdString(std::string(config.value<std::string_view>("username", "prosperoclient")));

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

    config.set_value<std::string_view>("username", m_username.toStdString());

    config.set_value<std::string_view>("public_key", ed25519::export_public_key(m_public_key));
    config.set_value<std::string_view>("private_key", ed25519::export_private_key(m_private_key));

    config.write(m_config_path);
}
