// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: highlighter.hh; Created: Fri Nov 28 2025 15:24:37
// Description: Chat syntax highlighter

#ifndef CLIENT_HIGHLIGHTER_HH
#define CLIENT_HIGHLIGHTER_HH 1
#pragma once

class Highlighter : public QSyntaxHighlighter {
    Q_OBJECT
    Q_PROPERTY(QQuickTextDocument* target READ target WRITE set_target)
    Q_PROPERTY(QColor mention_color READ mention_color WRITE set_mention_color)
    Q_PROPERTY(QColor quotation_color READ quotation_color WRITE set_quotation_color)

public:
    explicit Highlighter(QObject* parent = nullptr);
    virtual ~Highlighter(void) override = default;

    virtual void highlightBlock(const QString& text) override;

    QQuickTextDocument* target(void) const;
    void set_target(QQuickTextDocument* target);

    const QColor& mention_color(void) const;
    void set_mention_color(const QColor& color);

    const QColor& quotation_color(void) const;
    void set_quotation_color(const QColor& color);

private:
    QTextCharFormat m_quotation_format;
    QTextCharFormat m_mention_format;

    QRegularExpression m_quotation_pattern;
    QRegularExpression m_mention_pattern;
};

#endif
