#include "mainwindow.h"
#include "settings.h"
#include "thememanager.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("VoidTeX");
    QCoreApplication::setApplicationName("VoidTeX");

    QSettings s;
    // const QString themeName = s.value(SettingsKeys::ThemeName, "dark").toString();
    const QString themeName = s.value(SettingsKeys::ThemeName, "pure_void").toString();
    QSettings().setValue(SettingsKeys::CurrentLineMode, "lineNumberOnly"); // lineNumberOnly
    ThemeManager::instance().loadTheme(themeName);

    MainWindow w;
    w.show();
    return QApplication::exec();
}