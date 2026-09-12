#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QDockWidget;
class QPlainTextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onNew();
    void onOpen();
    void onSave();
    void onSaveAs();
    void onCompile();
    void onAbout();

private:
    void createActions();
    void createMenus();
    void createToolBar();
    void createCentralAndDocks();
    void createStatusBar();

    // Actions
    QAction *m_actionNew     = nullptr;
    QAction *m_actionOpen    = nullptr;
    QAction *m_actionSave    = nullptr;
    QAction *m_actionSaveAs  = nullptr;
    QAction *m_actionExit    = nullptr;
    QAction *m_actionCompile = nullptr;
    QAction *m_actionAbout   = nullptr;
    QAction *m_actionAboutQt = nullptr;

    // Widgets
    QPlainTextEdit *m_editor = nullptr;
    QPlainTextEdit *m_log    = nullptr;
    QDockWidget    *m_logDock = nullptr;
};

#endif // MAINWINDOW_H