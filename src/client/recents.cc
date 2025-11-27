// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: recents.cc; Created: Thu Nov 27 2025 16:11:00
// Description: Recent servers storage

#include "client/precompiled.hh"

#include "client/recents.hh"

#include "core/strtools.hh"

Recents* Recents::instance = nullptr;

Recents::Recents(QObject* parent) : QObject(parent)
{
    std::filesystem::path app_data_directory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString());
    std::filesystem::create_directories(app_data_directory);

    m_filepath = app_data_directory / "recents.txt";

    load_from_file();
}

const QString& Recents::latest(void) const
{
    return m_list.constFirst();
}

const QStringList& Recents::list(void) const
{
    return m_list;
}

void Recents::add(const QString& address)
{
    QStringList new_list;
    new_list.reserve(1U + m_list.size());

    for(qsizetype i = 0U; i < m_list.size(); ++i) {
        const auto& entry = m_list.at(m_list.size() - i - 1U);

        if(entry.compare(address)) {
            new_list.append(entry);
        }
    }

    new_list.append(address);

    m_list.assign(new_list.crbegin(), new_list.crend());

    emit updated();

    save_to_file();
}

void Recents::remove(const QString& address)
{
    m_list.removeAll(address);

    emit updated();

    save_to_file();
}

void Recents::load_from_file(void)
{
    std::ifstream stream(m_filepath, std::ios::in);
    std::string line;

    m_list.clear();

    if(stream.is_open()) {
        while(std::getline(stream, line)) {
            if(!strtools::is_whitespace(line)) {
                m_list.append(QString::fromStdString(line));
            }
        }
    }

    emit updated();
}

void Recents::save_to_file(void)
{
    std::ofstream stream(m_filepath, std::ios::out | std::ios::trunc);

    if(stream.is_open()) {
        for(const auto& entry : m_list) {
            stream << entry.toStdString() << std::endl;
        }
    }
}
