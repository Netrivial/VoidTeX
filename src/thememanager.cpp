#include "thememanager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDebug>

ThemeManager &ThemeManager::instance()
{
    static ThemeManager s;
    return s;
}

bool ThemeManager::loadTheme(const QString &name)
{
    const QString userDir = QStandardPaths::writableLocation(
                                QStandardPaths::AppConfigLocation) + "/themes";
    const QString userFile = QDir(userDir).filePath(name + ".json");
    if (loadFromFile(userFile))
        return true;

    const QString embedded = QStringLiteral(":/themes/%1.json").arg(name);
    if (loadFromFile(embedded))
        return true;

    qWarning() << "Theme not found:" << name;
    return false;
}

bool ThemeManager::loadFromFile(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }

    const QByteArray data = f.readAll();
    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "Theme JSON parse error:" << path << err.errorString();
        return false;
    }

    const QJsonObject root = doc.object();

    m_name = root.value("name").toString(QStringLiteral("Unnamed"));

    m_colors.clear();
    const QJsonObject colors = root.value("colors").toObject();
    for (auto it = colors.begin(); it != colors.end(); ++it) {
        const QColor c(it.value().toString());
        if (c.isValid())
            m_colors.insert(it.key(), c);
        else
            qWarning() << "Invalid color in theme:" << it.key()
                       << it.value().toString();
    }

    m_sizes.clear();
    const QJsonObject sizes = root.value("sizes").toObject();
    for (auto it = sizes.begin(); it != sizes.end(); ++it) {
        m_sizes.insert(it.key(), it.value().toInt());
    }

    emit themeChanged();
    return true;
}

QColor ThemeManager::color(const QString &key) const
{
    return m_colors.value(key, QColor(Qt::magenta));
}

int ThemeManager::size(const QString &key) const
{
    return m_sizes.value(key, 0);
}