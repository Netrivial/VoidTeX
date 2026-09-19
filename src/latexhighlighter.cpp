#include "latexhighlighter.h"
#include "thememanager.h"

#include <QTextDocument>

LatexHighlighter::LatexHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &LatexHighlighter::onThemeChanged);

    rebuildRules();
}

void LatexHighlighter::onThemeChanged()
{
    rebuildRules();
    rehighlight();
}

void LatexHighlighter::rebuildRules()
{
    auto &tm = ThemeManager::instance();
    m_rules.clear();

    // --- 1. Команды: \word, \word*, \word@inside ---
    {
        Rule r;
        // \ + одна или более латинских букв/@ (LaTeX внутренние команды) + опциональная *
        r.pattern = QRegularExpression(R"(\\[a-zA-Z@]+\*?)");
        r.format.setForeground(tm.color("latexCommand"));
        m_rules.append(r);
    }

    // --- 2. Скобки ---
    {
        Rule r;
        r.pattern = QRegularExpression(R"([{}])");
        r.format.setForeground(tm.color("latexBrace"));
        m_rules.append(r);
    }

    // --- 3. Комментарии (применяем последними — перекрывают всё) ---
    {
        Rule r;
        // % не предшествуемый \ — до конца блока
        r.pattern = QRegularExpression(R"((?<!\\)%.*)");
        r.format.setForeground(tm.color("latexComment"));
        r.format.setFontItalic(true);
        m_rules.append(r);
    }

    // --- 4. Математика $...$ (обрабатывается отдельно) ---
    // Одиночный $, не предшествуемый $ или \ и не сопровождаемый $
    m_mathDelimiter = QRegularExpression(R"((?<![\$\\])\$(?!\$))");
    m_mathFormat.setForeground(tm.color("latexMath"));
}

void LatexHighlighter::highlightBlock(const QString &text)
{
    // --- 1. Применяем однострочные правила ---
    for (const Rule &rule : m_rules) {
        auto it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            const auto match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // --- 2. Математика $...$ с поддержкой многострочности ---
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(m_mathDelimiter);

    while (startIndex >= 0) {
        const auto endMatch = m_mathDelimiter.match(text, startIndex + 1);
        int endIndex = endMatch.capturedStart();
        int length;

        if (endIndex == -1) {
            setCurrentBlockState(1);
            length = text.length() - startIndex;
        } else {
            length = endIndex - startIndex + endMatch.capturedLength();
        }

        setFormat(startIndex, length, m_mathFormat);
        startIndex = text.indexOf(m_mathDelimiter, startIndex + length);
    }
}