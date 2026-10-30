#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "compiler.h"
#include "logpanel.h"

#include <QMainWindow>
#include <QStringList>

class WelcomePage;
class QStackedWidget;
class QAction;
class QDockWidget;
class QMenu;
class QToolBar;
class SettingsDialog;
class QPlainTextEdit;
class Editor;
class Compiler;
class PdfPreview;
class FileTree;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void onNew();
    void onOpen();
    void onSave();
    void onSaveAs();
    void onClose();
    void onOpenRecent();
    void onCompile();
    void onAbout();
    void onDocumentModifiedChanged(bool modified);
    void onCompilerStarted(const QString &program, const QStringList &arguments);
    void onCompilerOutput(const QString &line);
    void onCompilerFinished(const Compiler::Result &result);
    void onCompilerMessage(const Compiler::Message &msg);
    void onSettings();
    void onOpenFolder();

private:
    void createActions();
    void createMenus();
    void createToolBar();
    void createCentralAndDocks();
    void createStatusBar();

    void saveWindowState();
    void restoreWindowState();

    bool maybeSave();
    bool loadFile(const QString &path);
    bool saveToDisk(const QString &path);
    void addToRecentFiles(const QString &path);
    void updateRecentFilesMenu();
    void updateWindowTitle();

    void showPdfPreview(const QString &pdfPath);
    void clearPdfPreview();

    void setProjectFolder(const QString &path);
    QString m_currentProjectDir;
    QString m_rootFilePath;

    // Actions
    QAction *m_actionNew            = nullptr;
    QAction *m_actionOpen           = nullptr;
    QAction *m_actionSave           = nullptr;
    QAction *m_actionSaveAs         = nullptr;
    QAction *m_actionClose          = nullptr;
    QAction *m_actionExit           = nullptr;
    QAction *m_actionCompile        = nullptr;
    QAction *m_actionAbout          = nullptr;
    QAction *m_actionAboutQt        = nullptr;
    QAction *m_actionOpenPdf        = nullptr;
    QAction *m_actionSettings       = nullptr;
    QAction *m_actionOpenFolder     = nullptr;

    // Widgets
    Editor         *m_editor        = nullptr;
    LogPanel       *m_log           = nullptr;
    QDockWidget    *m_logDock       = nullptr;
    Compiler       *m_compiler      = nullptr;
    PdfPreview     *m_pdfPreview    = nullptr;
    QDockWidget    *m_pdfDock       = nullptr;
    QDockWidget    *m_projectDock   = nullptr;
    QStackedWidget *m_centralStack  = nullptr;

    // Toolbar
    QToolBar *m_mainToolBar         = nullptr;

    // Menu
    QMenu    *m_recentFilesMenu     = nullptr;

    // Состояние файла
    QString     m_currentFilePath;
    QStringList m_recentFiles;

    //Welcome page
    WelcomePage    *m_welcomePage   = nullptr;

    // File tree
    FileTree       *m_fileTree      = nullptr;

    static constexpr int MaxRecentFiles = 10;
};

#endif // MAINWINDOW_H