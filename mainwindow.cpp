#include "mainwindow.h"
#include "editor.h"

#include <QAction>
#include <QApplication>
#include <QDockWidget>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QStatusBar>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("VoidTeX — untitled"));
    resize(1200, 800);

    createCentralAndDocks();
    createActions();
    createMenus();
    createToolBar();
    createStatusBar();
}

MainWindow::~MainWindow() = default;

void MainWindow::createCentralAndDocks()
{
    m_editor = new Editor(this);
    setCentralWidget(m_editor);

    m_log = new QPlainTextEdit(this);
    m_log->setReadOnly(true);
    m_log->setPlaceholderText(tr("Здесь будут сообщения компилятора..."));

    m_logDock = new QDockWidget(tr("Logs"), this);
    m_logDock->setObjectName("LogsDock");
    m_logDock->setWidget(m_log);
    m_logDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, m_logDock);
}

void MainWindow::createActions()
{
    // --- File ---
    m_actionNew = new QAction(tr("&New"), this);
    m_actionNew->setShortcut(QKeySequence::New);
    m_actionNew->setStatusTip(tr("Создать новый документ"));
    connect(m_actionNew, &QAction::triggered, this, &MainWindow::onNew);

    m_actionOpen = new QAction(tr("&Open..."), this);
    m_actionOpen->setShortcut(QKeySequence::Open);
    m_actionOpen->setStatusTip(tr("Открыть файл .tex"));
    connect(m_actionOpen, &QAction::triggered, this, &MainWindow::onOpen);

    m_actionSave = new QAction(tr("&Save"), this);
    m_actionSave->setShortcut(QKeySequence::Save);
    connect(m_actionSave, &QAction::triggered, this, &MainWindow::onSave);

    m_actionSaveAs = new QAction(tr("Save &As..."), this);
    m_actionSaveAs->setShortcut(QKeySequence::SaveAs);
    connect(m_actionSaveAs, &QAction::triggered, this, &MainWindow::onSaveAs);

    m_actionExit = new QAction(tr("E&xit"), this);
    m_actionExit->setShortcut(QKeySequence::Quit);
    connect(m_actionExit, &QAction::triggered, this, &QWidget::close);

    // --- Build ---
    m_actionCompile = new QAction(tr("&Compile"), this);
    m_actionCompile->setShortcut(QKeySequence(Qt::Key_F5));
    m_actionCompile->setStatusTip(tr("Скомпилировать документ"));
    connect(m_actionCompile, &QAction::triggered, this, &MainWindow::onCompile);

    // --- Help ---
    m_actionAbout = new QAction(tr("&About VoidTeX"), this);
    connect(m_actionAbout, &QAction::triggered, this, &MainWindow::onAbout);

    m_actionAboutQt = new QAction(tr("About &Qt"), this);
    connect(m_actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_actionNew);
    fileMenu->addAction(m_actionOpen);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actionSave);
    fileMenu->addAction(m_actionSaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actionExit);

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    QMenu *buildMenu = menuBar()->addMenu(tr("&Build"));
    buildMenu->addAction(m_actionCompile);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(m_logDock->toggleViewAction());

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(m_actionAbout);
    helpMenu->addAction(m_actionAboutQt);
}

void MainWindow::createToolBar()
{
    QToolBar *tb = addToolBar(tr("Main"));
    tb->setObjectName("MainToolBar");
    tb->setMovable(false);

    tb->addAction(m_actionNew);
    tb->addAction(m_actionOpen);
    tb->addAction(m_actionSave);
    tb->addSeparator();
    tb->addAction(m_actionCompile);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::onNew()      { m_log->appendPlainText(tr("[stub] New")); }
void MainWindow::onOpen()     { m_log->appendPlainText(tr("[stub] Open")); }
void MainWindow::onSave()     { m_log->appendPlainText(tr("[stub] Save")); }
void MainWindow::onSaveAs()   { m_log->appendPlainText(tr("[stub] Save As")); }
void MainWindow::onCompile()  { m_log->appendPlainText(tr("[stub] Compile")); }

void MainWindow::onAbout()
{
    QMessageBox::about(this,
                       tr("About VoidTeX"),
                       tr("<b>VoidTeX</b> — кроссплатформенный редактор LaTeX на Qt 6.<br>"
                          "Учебный проект."));
}