#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QColor>
#include <QHash>
#include <QObject>
#include <QString>

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    static ThemeManager &instance();

    bool loadTheme(const QString &name);

    QString currentThemeName() const { return m_name; }

    QColor color(const QString &key) const;
    int    size (const QString &key) const;

signals:
    void themeChanged();

private:
    ThemeManager() = default;
    ThemeManager(const ThemeManager &) = delete;
    ThemeManager &operator=(const ThemeManager &) = delete;

    bool loadFromFile(const QString &path);

    QString m_name;
    QHash<QString, QColor> m_colors;
    QHash<QString, int>    m_sizes;
};

#endif // THEMEMANAGER_H