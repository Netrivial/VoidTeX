#include "mainwindow.h"
#include "settingsmanager.h"
#include "thememanager.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("VoidTeX");
    QCoreApplication::setApplicationName("VoidTeX");

    const QString themeName =
        SettingsManager::instance().stringValue(SettingsKeys::ThemeName, "dark");
    ThemeManager::instance().loadTheme(themeName);

    SettingsManager::instance().setValue(SettingsKeys::ThemeName, themeName);


    MainWindow w;
    w.show();
    SettingsManager::instance().sync();
    return QApplication::exec();
}