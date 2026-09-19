#ifndef LATEXHIGHLIGHTER_H
#define LATEXHIGHLIGHTER_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>

class LatexHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit LatexHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private slots:
    void onThemeChanged();

private:
    struct Rule
    {
        QRegularExpression pattern;
        QTextCharFormat    format;
    };

    void rebuildRules();

    QVector<Rule> m_rules;

    QRegularExpression m_mathDelimiter;
    QTextCharFormat    m_mathFormat;
};

#endif // LATEXHIGHLIGHTER_H