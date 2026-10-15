#include "thememanager.h"

#include <QCoreApplication>
#include <QApplication>
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

    applyQss();

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

void ThemeManager::applyQss()
{
    const QString bg        = color("editorBg").name();
    const QString fg        = color("editorFg").name();
    const QString panelBg   = color("lineNumberBg").name();
    const QString border    = color("lineNumberBorder").name();
    const QString hover     = color("currentLineBg").name();
    const QString accent    = color("latexCommand").name();

    const QString qss = QString(R"(
/* ==== Основные окна ==== */
QMainWindow, QDialog {
    background-color: %1;
    color: %2;
}

/* ==== Док-панели ==== */
QDockWidget {
    background-color: %3;
    color: %2;
    titlebar-close-icon: none;
    titlebar-normal-icon: none;
}
QDockWidget::title {
    background-color: %3;
    color: %2;
    padding: 6px;
    border-bottom: 1px solid %4;
}

/* ==== Панель логов (QPlainTextEdit с objectName) ==== */
#LogPanel {
    background-color: %1;
    color: %2;
    border: none;
}

/* ==== Меню ==== */
QMenuBar {
    background-color: %3;
    color: %2;
}
QMenuBar::item {
    background: transparent;
    padding: 4px 10px;
}
QMenuBar::item:selected {
    background-color: %5;
}
QMenu {
    background-color: %3;
    color: %2;
    border: 1px solid %4;
}
QMenu::item {
    padding: 4px 24px;
}
QMenu::item:selected {
    background-color: %5;
}
QMenu::separator {
    height: 1px;
    background: %4;
    margin: 4px 8px;
}

/* ==== Тулбар и статусбар ==== */
QToolBar {
    background-color: %3;
    border: none;
    spacing: 4px;
    padding: 2px;
}
QToolButton {
    background: transparent;
    color: %2;
    padding: 4px 8px;
    border-radius: 4px;
}
QToolButton:hover {
    background-color: %5;
}
QToolButton:pressed {
    background-color: %6;
}
QStatusBar {
    background-color: %3;
    color: %2;
}

/* ==== Тонкие скроллбары с закруглением ==== */
QScrollBar:vertical {
    background: transparent;
    width: 8px;
    margin: 0;
    border: none;
}
QScrollBar::handle:vertical {
    background: %4;
    min-height: 24px;
    border-radius: 4px;
    margin: 1px;
}
QScrollBar::handle:vertical:hover {
    background: %2;
}
QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
    height: 0;
    background: none;
}
QScrollBar::add-page:vertical,
QScrollBar::sub-page:vertical {
    background: none;
}

QScrollBar:horizontal {
    background: transparent;
    height: 8px;
    margin: 0;
    border: none;
}
QScrollBar::handle:horizontal {
    background: %4;
    min-width: 24px;
    border-radius: 4px;
    margin: 1px;
}
QScrollBar::handle:horizontal:hover {
    background: %2;
}
QScrollBar::add-line:horizontal,
QScrollBar::sub-line:horizontal {
    width: 0;
    background: none;
}
QScrollBar::add-page:horizontal,
QScrollBar::sub-page:horizontal {
    background: none;
}

/* ==== Уголок между скроллбарами (напр., в PDF Preview) ==== */
QAbstractScrollArea::corner {
    background: %3;
    border: none;
}

/* ==== Вкладки (для будущего SettingsDialog) ==== */
QTabWidget::pane {
    border: 1px solid %4;
}
QTabBar::tab {
    background: %3;
    color: %2;
    padding: 6px 12px;
}
QTabBar::tab:selected {
    background: %1;
    color: %2;
    border-bottom: 2px solid %6;
}
)").arg(bg, fg, panelBg, border, hover, accent);

    qApp->setStyleSheet(qss);
}