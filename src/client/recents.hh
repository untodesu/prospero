// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: recents.hh; Created: Thu Nov 27 2025 16:08:30
// Description: Recent servers storage

#ifndef CLIENT_RECENTS_HH
#define CLIENT_RECENTS_HH 1
#pragma once

class Recents final : public QObject {
public:
    Q_OBJECT
    Q_PROPERTY(QString latest READ latest NOTIFY updated)
    Q_PROPERTY(QStringList list READ list NOTIFY updated)

public:
    static Recents* instance;

    explicit Recents(QObject* parent);
    virtual ~Recents(void) override = default;

    const QString& latest(void) const;
    const QStringList& list(void) const;

    Q_INVOKABLE void add(const QString& address);
    Q_INVOKABLE void remove(const QString& address);

signals:
    void updated(void);

private:
    void load_from_file(void);
    void save_to_file(void);

    std::filesystem::path m_filepath;
    QStringList m_list;
};

#endif
