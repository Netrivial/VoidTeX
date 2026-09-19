#ifndef EDITOR_H
#define EDITOR_H

#include "src/settings.h"

#include <QPlainTextEdit>

class Editor;

class LatexHighlighter;

class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(Editor *editor);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Editor *m_editor;
};

class Editor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr);

    int  lineNumberAreaWidth() const;
    void lineNumberAreaPaintEvent(QPaintEvent *event);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);
    void highlightCurrentLine();

private:
    LineNumberArea    *m_lineNumberArea;
    LatexHighlighter  *m_highlighter = nullptr;
    CurrentLineHighlight m_highlightMode = CurrentLineHighlight::Both;
};

#endif // EDITOR_H