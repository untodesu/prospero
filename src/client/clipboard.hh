// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: clipboard.hh; Created: Tue Nov 25 2025 11:52:30
// Description: Clipboard wrapper for QML since QClipboard is not directly accessible

#ifndef CLIENT_CLIPBOARD_HH
#define CLIENT_CLIPBOARD_HH 1
#pragma once

class Clipboard final : public QObject {
    Q_OBJECT

public:
    static Clipboard* instance;

    explicit Clipboard(QObject* parent = nullptr);
    virtual ~Clipboard(void) override = default;

    Q_INVOKABLE QString text(void) const;
    Q_INVOKABLE void set_text(const QString& text);

private:
    QClipboard* m_clipboard;
};

#endif
