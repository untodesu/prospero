// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: clipboard.cc; Created: Tue Nov 25 2025 11:53:51
// Description: Clipboard wrapper for QML since QClipboard is not directly accessible

#include "client/precompiled.hh"

#include "client/clipboard.hh"

Clipboard* Clipboard::instance = nullptr;

Clipboard::Clipboard(QObject* parent) : QObject(parent)
{
    m_clipboard = QApplication::clipboard();
}

QString Clipboard::text(void) const
{
    return m_clipboard->text();
}

void Clipboard::set_text(const QString& text)
{
    m_clipboard->setText(text);
}
