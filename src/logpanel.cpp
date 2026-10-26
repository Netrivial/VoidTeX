#include "logpanel.h"
#include "thememanager.h"
#include <QMouseEvent>
#include <QTextBlock>
#include <QScrollBar>

LogPanel::LogPanel(QWidget *parent)
    : QPlainTextEdit(parent)
{
    setReadOnly(true);
    setPlaceholderText(tr("Здесь будут сообщения компилятора..."));
    setObjectName("LogPanel");

    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, [this]() {
        clear();
    });
}

void LogPanel::appendInfo(const QString &text)
{
    appendStyledText(text, ThemeManager::instance().color("editorFg"), {});
}

void LogPanel::appendMessage(const Compiler::Message &msg)
{
    QColor color;
    switch (msg.type) {
    case Compiler::MessageType::Error:   color = ThemeManager::instance().color("latexCommand"); break;
    case Compiler::MessageType::Warning: color = QColor("orange"); break;
    default:                             color = ThemeManager::instance().color("editorFg"); break;
    }
    appendStyledText(msg.rawLine, color, msg);
}

void LogPanel::appendStyledText(const QString &text, const QColor &color, const Compiler::Message &msg)
{
    QTextCursor cursor(document());
    cursor.movePosition(QTextCursor::End);

    if (!document()->isEmpty()) {
        cursor.insertBlock();
    }

    QTextCharFormat fmt;
    fmt.setForeground(color);

    if (msg.type == Compiler::MessageType::Error) {
        fmt.setFontWeight(QFont::Bold);
        fmt.setFontUnderline(true);

        m_messages[cursor.blockNumber()] = msg;
    }

    cursor.insertText(text, fmt);
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

void LogPanel::mousePressEvent(QMouseEvent *e)
{
    QTextCursor cursor = cursorForPosition(e->pos());
    int blockNum = cursor.blockNumber();

    if (m_messages.contains(blockNum)) {
        const auto &msg = m_messages.value(blockNum);
        if (msg.line > 0) {
            emit messageClicked(msg.file, msg.line);
        }
    }

    QPlainTextEdit::mousePressEvent(e);
}