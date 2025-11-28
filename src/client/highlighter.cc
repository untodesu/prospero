// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: highlighter.cc; Created: Fri Nov 28 2025 15:27:00
// Description: Chat syntax highlighter

#include "client/precompiled.hh"

#include "client/highlighter.hh"

#include "core/exception.hh"

Highlighter::Highlighter(QObject* parent) : QSyntaxHighlighter(parent)
{
    m_mention_format.setFontUnderline(true);
    m_mention_format.setFontWeight(QFont::Bold);

    m_quote_format.setFontWeight(QFont::Normal);

    m_mention_pattern.setPattern("@[a-zA-Z0-9_\\-\\.]+");
    m_quote_pattern.setPattern("^\\s*>.*$");
}

void Highlighter::highlightBlock(const QString& text)
{
    QRegularExpressionMatchIterator mention_iterator(m_mention_pattern.globalMatch(text));
    QRegularExpressionMatchIterator quote_iterator(m_quote_pattern.globalMatch(text));

    while(mention_iterator.hasNext()) {
        QRegularExpressionMatch match(mention_iterator.next());
        setFormat(match.capturedStart(), match.capturedLength(), m_mention_format);
    }

    while(quote_iterator.hasNext()) {
        QRegularExpressionMatch match(quote_iterator.next());
        setFormat(match.capturedStart(), match.capturedLength(), m_quote_format);
    }
}

QQuickTextDocument* Highlighter::target(void) const
{
    auto document = this->document();
    auto parent = document ? document->parent() : nullptr;
    return qobject_cast<QQuickTextDocument*>(parent);
}

void Highlighter::set_target(QQuickTextDocument* target)
{
    setDocument(target ? target->textDocument() : nullptr);
}

const QColor& Highlighter::quote_color(void) const
{
    return m_quote_format.foreground().color();
}

void Highlighter::set_quote_color(const QColor& color)
{
    m_quote_format.setForeground(color);

    rehighlight();
}
