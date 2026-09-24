#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "compiler.h"

#include <QMainWindow>
#include <QStringList>

class QAction;
class QDockWidget;
class QMenu;
class QPlainTextEdit;
class Editor;
class Compiler;

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

private:
    void createActions();
    void createMenus();
    void createToolBar();
    void createCentralAndDocks();
    void createStatusBar();

    bool maybeSave();
    bool loadFile(const QString &path);
    bool saveToDisk(const QString &path);
    void addToRecentFiles(const QString &path);
    void updateRecentFilesMenu();
    void updateWindowTitle();

    // Actions
    QAction *m_actionNew     = nullptr;
    QAction *m_actionOpen    = nullptr;
    QAction *m_actionSave    = nullptr;
    QAction *m_actionSaveAs  = nullptr;
    QAction *m_actionClose   = nullptr;
    QAction *m_actionExit    = nullptr;
    QAction *m_actionCompile = nullptr;
    QAction *m_actionAbout   = nullptr;
    QAction *m_actionAboutQt = nullptr;

    // Widgets
    Editor         *m_editor = nullptr;
    QPlainTextEdit *m_log    = nullptr;
    QDockWidget    *m_logDock = nullptr;
    Compiler *m_compiler = nullptr;

    // Меню
    QMenu *m_recentFilesMenu = nullptr;

    // Состояние файла
    QString     m_currentFilePath;
    QStringList m_recentFiles;

    static constexpr int MaxRecentFiles = 10;
};

#endif // MAINWINDOW_H