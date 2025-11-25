// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: identity.cc; Created: Tue Nov 25 2025 12:01:57
// Description: Client ed25519 keypair

#include "client/precompiled.hh"

#include "client/identity.hh"

#include "core/buffer.hh"
#include "core/exception.hh"

Identity* Identity::instance = nullptr;

Identity::Identity(QObject* parent) : QObject(parent)
{
    std::filesystem::path app_data_directory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString());
    std::filesystem::create_directories(app_data_directory);

    m_filepath = app_data_directory / "identity.dat";

    if(!import_file(m_filepath)) {
        ed25519::seed_buffer seed;
        ed25519::generate_seed(seed);
        ed25519::generate_keys(seed, m_public_key, m_private_key);

        m_public_key_hexstring = QString::fromStdString(ed25519::export_public_key(m_public_key));

        if(!export_file(m_filepath)) {
            throw core::runtime_error("cannot export identity keypair to {}", m_filepath.string());
        }
    }
}

const QString& Identity::public_key_hexstring(void) const
{
    return m_public_key_hexstring;
}

void Identity::import_keypair(void)
{
    auto title = tr("Import client identity");
    auto start_dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    auto filter = tr("Identity Files (*.dat);;All Files (*)");
    auto path = QFileDialog::getOpenFileName(nullptr, title, start_dir, filter);

    if(path.size()) {
        if(!import_file(path.toStdString())) {
            auto title = tr("Import Error");
            auto message = tr("The selected file is not a valid identity file");
            QMessageBox::warning(nullptr, title, message);
        }
    }
}

void Identity::export_keypair(void) const
{
    auto title = tr("Export client identity");
    auto start_dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    auto filter = tr("Identity Files (*.dat);;All Files (*)");
    auto path = QFileDialog::getSaveFileName(nullptr, title, start_dir, filter);

    if(path.size()) {
        if(!export_file(path.toStdString())) {
            auto title = tr("Export Error");
            auto message = tr("Cannot export identity to the selected file");
            QMessageBox::warning(nullptr, title, message);
        }
    }
}

void Identity::generate_keypair(void)
{
    auto title = tr("Generate new identity");
    auto message = tr("Generating new identity will replace the current one. Are you sure?");
    auto response = QMessageBox::question(nullptr, title, message, QMessageBox::Yes | QMessageBox::No);

    if(response == QMessageBox::Yes) {
        ed25519::seed_buffer seed;
        ed25519::generate_seed(seed);
        ed25519::generate_keys(seed, m_public_key, m_private_key);

        m_public_key_hexstring = QString::fromStdString(ed25519::export_public_key(m_public_key));

        if(!export_file(m_filepath)) {
            throw core::runtime_error("cannot export identity keypair to {}", m_filepath.string());
        }

        emit keypair_changed();
    }
}

bool Identity::import_file(const std::filesystem::path& filepath)
{
    std::ifstream stream(filepath, std::ios::binary | std::ios::in);

    if(stream.is_open()) {
        ReadBuffer buffer(stream);

        if(buffer.size() == m_private_key.size() + m_public_key.size()) {
            buffer.read(m_private_key.data(), m_private_key.size());
            buffer.read(m_public_key.data(), m_public_key.size());

            m_public_key_hexstring = QString::fromStdString(ed25519::export_public_key(m_public_key));

            emit keypair_changed();

            return true;
        }
    }

    return false;
}

bool Identity::export_file(const std::filesystem::path& filepath) const
{
    WriteBuffer buffer;
    buffer.write(m_private_key.data(), m_private_key.size());
    buffer.write(m_public_key.data(), m_public_key.size());

    std::ofstream stream(filepath, std::ios::binary | std::ios::out | std::ios::trunc);

    if(stream.is_open()) {
        buffer.to_stream(stream);

        return true;
    }

    return false;
}
