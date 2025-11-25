// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: version.hh; Created: Tue Nov 25 2025 11:46:45
// Description: Qt meta-system wrapper for version constants

#ifndef CLIENT_VERSION_HH
#define CLIENT_VERSION_HH 1
#pragma once

class Version final : public QObject {
    Q_OBJECT
    Q_PROPERTY(unsigned int major READ major CONSTANT)
    Q_PROPERTY(unsigned int minor READ minor CONSTANT)
    Q_PROPERTY(unsigned int patch READ patch CONSTANT)
    Q_PROPERTY(QString branch READ branch CONSTANT)
    Q_PROPERTY(QString commit READ commit CONSTANT)
    Q_PROPERTY(QString semver READ semver CONSTANT)
    Q_PROPERTY(QString full READ full CONSTANT)

public:
    static Version* instance;

    explicit Version(QObject* parent = nullptr);
    virtual ~Version(void) override = default;

    unsigned int major(void) const;
    unsigned int minor(void) const;
    unsigned int patch(void) const;
    const QString& branch(void) const;
    const QString& commit(void) const;
    const QString& semver(void) const;
    const QString& full(void) const;

private:
    QString m_branch;
    QString m_commit;
    QString m_semver;
    QString m_full;
};

#endif
