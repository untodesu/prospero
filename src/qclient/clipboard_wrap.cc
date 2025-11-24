// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: clipboard_wrap.cc; Created: Tue Nov 25 2025 01:34:10
// Description: Clipboard wrapper for QML integration

#include "qclient/precompiled.hh"

#include "qclient/clipboard_wrap.hh"

ClipboardWrap::ClipboardWrap(QObject* parent) : QObject(parent)
{
    m_clipboard = QGuiApplication::clipboard();
}

QString ClipboardWrap::getText(void) const
{
    return m_clipboard->text();
}

void ClipboardWrap::setText(const QString& text) const
{
    m_clipboard->setText(text);
}
