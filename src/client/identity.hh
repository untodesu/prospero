// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: identity.hh; Created: Tue Nov 25 2025 11:57:15
// Description: Client ed25519 keypair

#ifndef CLIENT_IDENTITY_HH
#define CLIENT_IDENTITY_HH 1
#pragma once

#include "core/ed25519.hh"

class Identity final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString public_key READ public_key_hexstring CONSTANT)

public:
    static Identity* instance;

    explicit Identity(QObject* parent = nullptr);
    virtual ~Identity(void) override = default;

    constexpr const ed25519::pkey_buffer& public_key(void) const;
    constexpr const ed25519::skey_buffer& private_key(void) const;

    const QString& public_key_hexstring(void) const;

public slots:
    void import_keypair(void);
    void export_keypair(void) const;
    void generate_keypair(void);

signals:
    void keypair_changed(void);

private:
    bool import_file(const std::filesystem::path& filepath);
    bool export_file(const std::filesystem::path& filepath) const;

    std::filesystem::path m_filepath;
    ed25519::pkey_buffer m_public_key;
    ed25519::skey_buffer m_private_key;
    QString m_public_key_hexstring;
};

constexpr const ed25519::pkey_buffer& Identity::public_key(void) const
{
    return m_public_key;
}

constexpr const ed25519::skey_buffer& Identity::private_key(void) const
{
    return m_private_key;
}

#endif
