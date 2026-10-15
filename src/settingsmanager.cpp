#include "settingsmanager.h"

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

SettingsManager &SettingsManager::instance()
{
    static SettingsManager s;
    return s;
}

SettingsManager::SettingsManager()
    : m_settings(
          QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation))
              .filePath("settings.ini"),
          QSettings::IniFormat)
{
    QDir().mkpath(QFileInfo(m_settings.fileName()).absolutePath());
}

QVariant SettingsManager::value(const QString &key, const QVariant &defaultValue) const
{
    return m_settings.value(key, defaultValue);
}

void SettingsManager::setValue(const QString &key, const QVariant &value)
{
    if (m_settings.value(key) == value)
        return;

    m_settings.setValue(key, value);
    emit settingChanged(key, value);
}

QString SettingsManager::stringValue(const QString &key, const QString &def) const
{
    return m_settings.value(key, def).toString();
}

int SettingsManager::intValue(const QString &key, int def) const
{
    return m_settings.value(key, def).toInt();
}

bool SettingsManager::boolValue(const QString &key, bool def) const
{
    return m_settings.value(key, def).toBool();
}

void SettingsManager::sync()
{
    m_settings.sync();
}