#include "mainwindow.h"
#include "src/settings.h"
#include "src/thememanager.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("VoidTeX");
    QCoreApplication::setApplicationName("VoidTeX");

    QSettings s;
    // const QString themeName = s.value(SettingsKeys::ThemeName, "dark").toString();
    const QString themeName = s.value(SettingsKeys::ThemeName, "light").toString();
    QSettings().setValue(SettingsKeys::CurrentLineMode, "both");
    ThemeManager::instance().loadTheme(themeName);

    MainWindow w;
    w.show();
    return QApplication::exec();
}