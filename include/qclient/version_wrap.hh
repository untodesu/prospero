// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: version_wrap.hh; Created: Tue Nov 25 2025 01:29:26
// Description: Qt meta-object wrapper for version information
#ifndef QCLIENT_VERSION_WRAP_HH
#define QCLIENT_VERSION_WRAP_HH 1
#pragma once

class VersionWrap final : public QObject {
    Q_OBJECT
    Q_PROPERTY(unsigned int major READ major CONSTANT)
    Q_PROPERTY(unsigned int minor READ minor CONSTANT)
    Q_PROPERTY(unsigned int patch READ patch CONSTANT)
    Q_PROPERTY(QString branch READ branch CONSTANT)
    Q_PROPERTY(QString commit READ commit CONSTANT)
    Q_PROPERTY(QString semver READ semver CONSTANT)

public:
    explicit VersionWrap(QObject* parent = nullptr);
    virtual ~VersionWrap(void) override = default;

    unsigned int major(void) const noexcept;
    unsigned int minor(void) const noexcept;
    unsigned int patch(void) const noexcept;
    const QString& branch(void) const noexcept;
    const QString& commit(void) const noexcept;
    const QString& semver(void) const noexcept;

private:
    QString m_branch;
    QString m_commit;
    QString m_semver;
};

#endif // QCLIENT_VERSION_WRAP_HH
