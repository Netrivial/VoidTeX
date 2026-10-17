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
}
QDockWidget::title {
    background-color: %3;
    color: %2;
    padding: 8px 12px;
    border-bottom: 1px solid %4;
    font-weight: bold;
}
QDockWidget::close-button,
QDockWidget::float-button {
    background: transparent;
    border: none;
    icon-size: 25px;
    padding: 2px;
    margin: 2px;
}
QDockWidget::close-button:hover,
QDockWidget::float-button:hover {
    background: %5;
    border-radius: 3px;
}

/* ==== Панель логов (QPlainTextEdit с objectName) ==== */
#LogPanel {
    background-color: %1;
    color: %2;
    border: none;
}

/* ==== PDF View ==== */
QPdfView {
    border: none;
    background-color: %3;
}

/* ==== Меню ==== */
QMenu {
    background-color: %3;
    color: %2;
    border: 1px solid %4;
    min-width: 180px;
}
QMenu::item {
    padding: 6px 32px;
}
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

/* ==== Редактор ==== */
#Editor {
    background-color: %1;
    color: %2;
    border: none;
    selection-background-color: %6;
    selection-color: %2;
}

/* ==== Тулбар и статусбар ==== */
QToolBar {
    background-color: %3;
    border: none;
    spacing: 4px;
    padding: 2px;
}
QToolBar::separator {
    background: %4;
    width: 1px;
    margin: 4px 6px;
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
    width: 12px;
    margin: 0;
    border: none;
}
QScrollBar::handle:vertical {
    background: %5;
    min-height: 24px;
    border-radius: 6px;
    margin: 2px;
}
QScrollBar::handle:vertical:hover {
    background: %2;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    height: 0;
    background: none;
}
QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
    background: none;
}

QScrollBar:horizontal {
    background: transparent;
    height: 12px;
    margin: 0;
    border: none;
}
QScrollBar::handle:horizontal {
    background: %5;
    min-width: 24px;
    border-radius: 6px;
    margin: 2px;
}
QScrollBar::handle:horizontal:hover {
    background: %2;
}
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
    width: 0;
    background: none;
}
QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
    background: none;
}

/* ==== Уголок между скроллбарами (напр., в PDF Preview) ==== */
QAbstractScrollArea::corner {
    background: %3;
    border: none;
}

/* ==== Вкладки ==== */
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
/* ==== Диалоги — содержимое ==== */
QDialog QLabel {
    color: %2;
}
QDialog QCheckBox {
    color: %2;
}
QDialog QComboBox,
QDialog QSpinBox,
QDialog QDoubleSpinBox,
QDialog QLineEdit,
QDialog QPlainTextEdit {
    background-color: %1;
    color: %2;
    border: 1px solid %4;
    padding: 4px;
    border-radius: 4px;
    selection-background-color: %6;
}
QDialog QComboBox::drop-down {
    border: none;
    width: 20px;
}
QDialog QComboBox QAbstractItemView {
    background-color: %3;
    color: %2;
    selection-background-color: %5;
}
QDialog QPushButton {
    background-color: %3;
    color: %2;
    border: 1px solid %4;
    padding: 6px 14px;
    border-radius: 4px;
    min-width: 70px;
}
QDialog QPushButton:hover {
    background-color: %5;
}
QDialog QPushButton:pressed {
    background-color: %6;
}
QDialog QPushButton:default {
    border: 1px solid %6;
}
QCheckBox {
    color: %2;
    spacing: 8px;
}
QCheckBox::indicator {
    width: 16px;
    height: 16px;
    border: 1px solid %4;
    background: %1;
    border-radius: 3px;
}
QCheckBox::indicator:hover {
    border: 1px solid %5;
}
QCheckBox::indicator:checked {
    background: %6;
    border: 1px solid %6;
}
QCheckBox::indicator:disabled {
    background: %3;
    border: 1px solid %4;
}
)").arg(bg, fg, panelBg, border, hover, accent);

    qApp->setStyleSheet(qss);
}