#ifndef LOGPANEL_H
#define LOGPANEL_H

#include <QPlainTextEdit>
#include <QHash>
#include "compiler.h"

class LogPanel : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit LogPanel(QWidget *parent = nullptr);

    void appendMessage(const Compiler::Message &msg);
    void appendInfo(const QString &text);

signals:
    void messageClicked(const QString &file, int line);

protected:
    void mousePressEvent(QMouseEvent *e) override;

private:
    void appendStyledText(const QString &text, const QColor &color, const Compiler::Message &msg = {});

    QHash<int, Compiler::Message> m_messages;
};

#endif // LOGPANEL_H