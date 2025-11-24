// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: version_wrap.cc; Created: Tue Nov 25 2025 01:31:32
// Description: Qt meta-object wrapper for version information

#include "qclient/precompiled.hh"

#include "qclient/version_wrap.hh"

#include "core/version.hh"

VersionWrap::VersionWrap(QObject* parent)
{
    m_branch = QString::fromStdString(std::string(core::version_branch));
    m_commit = QString::fromStdString(std::string(core::version_commit));
    m_semver = QString::fromStdString(std::string(core::version_semver));
}

unsigned int VersionWrap::major(void) const noexcept
{
    return core::version_major;
}

unsigned int VersionWrap::minor(void) const noexcept
{
    return core::version_minor;
}

unsigned int VersionWrap::patch(void) const noexcept
{
    return core::version_patch;
}

const QString& VersionWrap::branch(void) const noexcept
{
    return m_branch;
}

const QString& VersionWrap::commit(void) const noexcept
{
    return m_commit;
}

const QString& VersionWrap::semver(void) const noexcept
{
    return m_semver;
}
