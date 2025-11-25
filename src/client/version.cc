// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: version.cc; Created: Tue Nov 25 2025 11:50:08
// Description: Qt meta-system wrapper for core::version constants

#include "client/precompiled.hh"

#include "client/version.hh"

#include "core/version.hh"

Version* Version::instance = nullptr;

Version::Version(QObject* parent) : QObject(parent)
{
    m_branch = QString::fromStdString(std::string(version::branch));
    m_commit = QString::fromStdString(std::string(version::commit));
    m_semver = QString::fromStdString(std::string(version::semver));
    m_full = QStringLiteral("%1-%2 [%3]").arg(m_semver, m_branch, m_commit);
}

unsigned int Version::major(void) const
{
    return version::major;
}

unsigned int Version::minor(void) const
{
    return version::minor;
}

unsigned int Version::patch(void) const
{
    return version::patch;
}

const QString& Version::branch(void) const
{
    return m_branch;
}

const QString& Version::commit(void) const
{
    return m_commit;
}

const QString& Version::semver(void) const
{
    return m_semver;
}

const QString& Version::full(void) const
{
    return m_full;
}
