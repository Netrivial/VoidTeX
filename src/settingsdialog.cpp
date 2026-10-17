#include "settingsdialog.h"
#include "settingsmanager.h"


#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Настройки — VoidTeX"));
    resize(640, 520);

    m_tabs = new QTabWidget(this);
    m_tabs->addTab(createGeneralTab(),  tr("Общие"));
    m_tabs->addTab(createEditorTab(),   tr("Редактор"));
    m_tabs->addTab(createPreviewTab(),  tr("Просмотр"));
    m_tabs->addTab(createBuildTab(),    tr("Сборка"));

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply,
        this);

    connect(buttons, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);
    connect(buttons->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &SettingsDialog::apply);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_tabs);
    layout->addWidget(buttons);

    loadFromSettings();
}

// ============ Вкладки ============

QWidget *SettingsDialog::createGeneralTab()
{
    auto *w = new QWidget;
    auto *form = new QFormLayout(w);

    m_themeCombo = new QComboBox;
    m_themeCombo->addItem("Dark",         "dark");
    m_themeCombo->addItem("Light",        "light");
    m_themeCombo->addItem("Pure Void",    "pure_void");
    m_themeCombo->addItem("Crimson Void", "crimson_void");
    form->addRow(tr("Тема оформления:"), m_themeCombo);

    return w;
}

QWidget *SettingsDialog::createEditorTab()
{
    auto *w = new QWidget;
    auto *form = new QFormLayout(w);

    m_lineModeCombo = new QComboBox;
    m_lineModeCombo->addItem(tr("Не подсвечивать"),            "none");
    m_lineModeCombo->addItem(tr("Только номер строки"),        "lineNumberOnly");
    m_lineModeCombo->addItem(tr("Только фон строки"),          "lineBackground");
    m_lineModeCombo->addItem(tr("Номер и фон"),                "both");
    form->addRow(tr("Подсветка текущей строки:"), m_lineModeCombo);

    m_zoomSpin = new QSpinBox;
    m_zoomSpin->setRange(50, 400);
    m_zoomSpin->setSuffix(" %");
    m_zoomSpin->setSingleStep(10);
    form->addRow(tr("Масштаб шрифта редактора:"), m_zoomSpin);

    m_templateEdit = new QPlainTextEdit;
    m_templateEdit->setPlaceholderText(tr("Шаблон для File → New"));
    m_templateEdit->setMinimumHeight(180);
    form->addRow(tr("Шаблон нового документа:"), m_templateEdit);

    return w;
}

QWidget *SettingsDialog::createPreviewTab()
{
    auto *w = new QWidget;
    auto *form = new QFormLayout(w);

    m_showPdfOnStartup = new QCheckBox(tr("Показывать PDF-превью при запуске"));
    form->addRow(m_showPdfOnStartup);

    return w;
}

QWidget *SettingsDialog::createBuildTab()
{
    auto *w = new QWidget;
    auto *form = new QFormLayout(w);

    m_autoCompile = new QCheckBox(tr("Автокомпиляция при сохранении файла"));
    form->addRow(m_autoCompile);

    return w;
}

// ============ Загрузка / сохранение ============

void SettingsDialog::loadFromSettings()
{
    auto &s = SettingsManager::instance();

    // General
    const QString theme = s.stringValue(SettingsKeys::ThemeName, "dark");
    const int themeIdx = m_themeCombo->findData(theme);
    if (themeIdx >= 0)
        m_themeCombo->setCurrentIndex(themeIdx);

    // Editor
    const QString lineMode = s.stringValue(SettingsKeys::CurrentLineMode, "both");
    const int lineIdx = m_lineModeCombo->findData(lineMode);
    if (lineIdx >= 0)
        m_lineModeCombo->setCurrentIndex(lineIdx);

    m_zoomSpin->setValue(s.intValue(SettingsKeys::EditorZoomPercent, 100));
    m_templateEdit->setPlainText(
        s.stringValue(SettingsKeys::NewFileTemplate, QString()));

    // Preview
    m_showPdfOnStartup->setChecked(
        s.boolValue(SettingsKeys::ShowPdfOnStartup, false));

    // Build
    m_autoCompile->setChecked(
        s.boolValue(SettingsKeys::AutoCompileOnSave, false));
}

void SettingsDialog::saveToSettings()
{
    auto &s = SettingsManager::instance();

    s.setValue(SettingsKeys::ThemeName,
               m_themeCombo->currentData().toString());

    s.setValue(SettingsKeys::CurrentLineMode,
               m_lineModeCombo->currentData().toString());
    s.setValue(SettingsKeys::EditorZoomPercent, m_zoomSpin->value());
    s.setValue(SettingsKeys::NewFileTemplate, m_templateEdit->toPlainText());

    s.setValue(SettingsKeys::ShowPdfOnStartup, m_showPdfOnStartup->isChecked());

    s.setValue(SettingsKeys::AutoCompileOnSave, m_autoCompile->isChecked());
}

void SettingsDialog::apply()
{
    saveToSettings();
}

void SettingsDialog::accept()
{
    saveToSettings();
    QDialog::accept();
}