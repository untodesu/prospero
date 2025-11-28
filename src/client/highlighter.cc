// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: highlighter.cc; Created: Fri Nov 28 2025 15:27:00
// Description: Chat syntax highlighter

#include "client/precompiled.hh"

#include "client/highlighter.hh"

#include "core/exception.hh"

Highlighter::Highlighter(QObject* parent) : QSyntaxHighlighter(parent)
{
    m_mention_pattern.setPattern("@[a-zA-Z0-9_\\-\\.]+");
    m_quotation_pattern.setPattern("^\\s*>.*$");
}

void Highlighter::highlightBlock(const QString& text)
{
    QRegularExpressionMatchIterator mention_iterator(m_mention_pattern.globalMatch(text));
    QRegularExpressionMatchIterator quotation_iterator(m_quotation_pattern.globalMatch(text));

    while(mention_iterator.hasNext()) {
        QRegularExpressionMatch match(mention_iterator.next());
        setFormat(match.capturedStart(), match.capturedLength(), m_mention_format);
    }

    while(quotation_iterator.hasNext()) {
        QRegularExpressionMatch match(quotation_iterator.next());
        setFormat(match.capturedStart(), match.capturedLength(), m_quotation_format);
    }
}

QQuickTextDocument* Highlighter::target(void) const
{
    return qobject_cast<QQuickTextDocument*>(document());
}

void Highlighter::set_target(QQuickTextDocument* target)
{
    setDocument(target ? target->textDocument() : nullptr);
}

const QColor& Highlighter::mention_color(void) const
{
    return m_mention_format.foreground().color();
}

void Highlighter::set_mention_color(const QColor& color)
{
    m_mention_format.setForeground(color);

    rehighlight();
}

const QColor& Highlighter::quotation_color(void) const
{
    return m_quotation_format.foreground().color();
}

void Highlighter::set_quotation_color(const QColor& color)
{
    m_quotation_format.setForeground(color);

    rehighlight();
}
