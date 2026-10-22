#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

class QComboBox;
class QCheckBox;
class QSpinBox;
class QDoubleSpinBox;
class QPlainTextEdit;
class QTabWidget;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private slots:
    void apply();
    void accept() override;

private:
    QWidget *createGeneralTab();
    QWidget *createEditorTab();
    QWidget *createPreviewTab();
    QWidget *createBuildTab();

    void loadFromSettings();
    void saveToSettings();

    QTabWidget     *m_tabs              = nullptr;

    // General
    QComboBox      *m_themeCombo        = nullptr;

    // Editor
    QComboBox      *m_lineModeCombo     = nullptr;
    QSpinBox       *m_zoomSpin          = nullptr;
    QPlainTextEdit *m_templateEdit      = nullptr;

    // Preview
    QCheckBox      *m_showPdfOnStartup  = nullptr;
    QCheckBox      *m_showPdfOnCompile  = nullptr;
    QCheckBox      *m_showLogsOnCompile = nullptr;

    // Welcome page
    QCheckBox      *m_showWelcome       = nullptr;

    // Build
    QCheckBox      *m_autoCompile       = nullptr;
};

#endif // SETTINGSDIALOG_H