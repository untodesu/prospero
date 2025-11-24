// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: clipboard_wrap.hh; Created: Tue Nov 25 2025 01:33:13
// Description: Clipboard wrapper for QML integration
#ifndef QCLIENT_CLIPBOARD_WRAP_HH
#define QCLIENT_CLIPBOARD_WRAP_HH 1
#pragma once

class ClipboardWrap final : public QObject {
    Q_OBJECT

public:
    explicit ClipboardWrap(QObject* parent = nullptr);
    virtual ~ClipboardWrap(void) override = default;

    Q_INVOKABLE QString getText(void) const;
    Q_INVOKABLE void setText(const QString& text) const;

private:
    QClipboard* m_clipboard;
};

#endif // QCLIENT_CLIPBOARD_WRAP_HH
