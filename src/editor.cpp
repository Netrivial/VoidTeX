#include "editor.h"
#include "thememanager.h"
#include "latexhighlighter.h"

#include <QFontDatabase>
#include <QPainter>
#include <QTextBlock>

// ---------- LineNumberArea ----------

LineNumberArea::LineNumberArea(Editor *editor)
    : QWidget(editor)
    , m_editor(editor)
{
}

QSize LineNumberArea::sizeHint() const
{
    return QSize(m_editor->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    m_editor->lineNumberAreaPaintEvent(event);
}

// ---------- Editor ----------

Editor::Editor(QWidget *parent)
    : QPlainTextEdit(parent)
    , m_lineNumberArea(new LineNumberArea(this))
{
    auto &tm = ThemeManager::instance();

    QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    f.setPointSize(tm.size("fontPointSize"));
    setFont(f);

    setTabStopDistance(tm.size("tabWidthInSpaces")
                       * fontMetrics().horizontalAdvance(QLatin1Char(' ')));

    QPalette pal = palette();
    pal.setColor(QPalette::Base,  tm.color("editorBg"));
    pal.setColor(QPalette::Text,  tm.color("editorFg"));
    pal.setColor(QPalette::Highlight, tm.color("editorSelection"));
    pal.setColor(QPalette::HighlightedText,  tm.color("editorSelectionFg"));
    setPalette(pal);

    QSettings s;
    m_highlightMode = currentLineHighlightFromString(
        s.value(SettingsKeys::CurrentLineMode, "both").toString());

    connect(this, &Editor::blockCountChanged,
            this, &Editor::updateLineNumberAreaWidth);
    connect(this, &Editor::updateRequest,
            this, &Editor::updateLineNumberArea);
    connect(this, &Editor::cursorPositionChanged,
            this, &Editor::highlightCurrentLine);

    m_highlighter = new LatexHighlighter(document());

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

int Editor::lineNumberAreaWidth() const
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) { max /= 10; ++digits; }

    const int padding = ThemeManager::instance().size("lineNumberPadding");
    const int space = padding + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void Editor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void Editor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy != 0) {
        m_lineNumberArea->scroll(0, dy);
    } else {
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void Editor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    const QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(
        QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void Editor::highlightCurrentLine()
{
    const bool wantBackground =
        (m_highlightMode == CurrentLineHighlight::LineBackground ||
         m_highlightMode == CurrentLineHighlight::Both);

    const bool wantNumber =
        (m_highlightMode == CurrentLineHighlight::LineNumberOnly ||
         m_highlightMode == CurrentLineHighlight::Both);

    QList<QTextEdit::ExtraSelection> selections;

    if (wantBackground && !isReadOnly()) {
        QTextEdit::ExtraSelection sel;
        sel.format.setBackground(ThemeManager::instance().color("currentLineBg"));
        sel.format.setProperty(QTextFormat::FullWidthSelection, true);
        sel.cursor = textCursor();
        sel.cursor.clearSelection();
        selections.append(sel);
    }

    setExtraSelections(selections);

    if (wantNumber || m_highlightMode == CurrentLineHighlight::None)
        m_lineNumberArea->update();
}

void Editor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    auto &tm = ThemeManager::instance();

    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), tm.color("lineNumberBg"));

    const bool highlightNumber =
        (m_highlightMode == CurrentLineHighlight::LineNumberOnly ||
         m_highlightMode == CurrentLineHighlight::Both);

    const int currentLine = textCursor().blockNumber();

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    const QColor normalColor = tm.color("lineNumberFg");
    const QColor activeColor = tm.color("currentLineNumberFg");

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const QString number = QString::number(blockNumber + 1);
            painter.setPen(highlightNumber && blockNumber == currentLine
                               ? activeColor
                               : normalColor);
            painter.drawText(0, top,
                             m_lineNumberArea->width() - 6,
                             fontMetrics().height(),
                             Qt::AlignRight,
                             number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}