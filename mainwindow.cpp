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
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

// start LaTeX code
namespace {
const char *kNewFileTemplate = R"(\documentclass{article}
\usepackage[utf8]{inputenc}
\usepackage[T2A]{fontenc}
\usepackage[russian]{babel}

\title{Новый документ}
\author{}
\date{\today}

\begin{document}
\maketitle

% Начните печатать здесь

\end{document}
)";
} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("VoidTeX"));
    resize(1200, 800);

    QSettings s;
    m_recentFiles = s.value("files/recent").toStringList();

    createCentralAndDocks();
    createActions();
    createMenus();
    createToolBar();
    createStatusBar();

    updateRecentFilesMenu();
    updateWindowTitle();

    // Drag and drop
    setAcceptDrops(true);
}

MainWindow::~MainWindow() = default;

void MainWindow::createCentralAndDocks()
{
    m_editor = new Editor(this);
    setCentralWidget(m_editor);
    connect(m_editor->document(), &QTextDocument::modificationChanged,
            this, &MainWindow::onDocumentModifiedChanged);

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

    m_actionClose = new QAction(tr("&Close"), this);
    m_actionClose->setShortcut(QKeySequence::Close);  // Ctrl+W / Cmd+W
    connect(m_actionClose, &QAction::triggered, this, &MainWindow::onClose);

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

void MainWindow::onNew()
{
    if (!maybeSave()) return;

    m_editor->setPlainText(QString::fromUtf8(kNewFileTemplate));
    m_editor->document()->setModified(false);
    m_currentFilePath.clear();
    updateWindowTitle();
}

void MainWindow::onOpen()
{
    if (!maybeSave()) return;

    const QString path = QFileDialog::getOpenFileName(this,
                                                      tr("Открыть документ"),
                                                      m_currentFilePath.isEmpty() ? QDir::homePath()
                                                                                  : QFileInfo(m_currentFilePath).absolutePath(),
                                                      tr("LaTeX документы (*.tex *.latex);;Все файлы (*)"));

    if (!path.isEmpty())
        loadFile(path);
}

void MainWindow::onSave()
{
    if (m_currentFilePath.isEmpty()) {
        onSaveAs();
        return;
    }
    saveToDisk(m_currentFilePath);
}

void MainWindow::onSaveAs()
{
    QString suggested = m_currentFilePath;
    if (suggested.isEmpty())
        suggested = QDir::homePath() + "/untitled.tex";

    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Сохранить как"),
                                                suggested,
                                                tr("LaTeX документы (*.tex *.latex);;Все файлы (*)"));

    if (path.isEmpty())
        return;

    if (QFileInfo(path).suffix().isEmpty())
        path += ".tex";

    saveToDisk(path);
}

void MainWindow::onCompile() { m_log->appendPlainText(tr("[stub] Compile")); }

void MainWindow::onAbout()
{
    QMessageBox::about(this,
                       tr("About VoidTeX"),
                       tr("<b>VoidTeX</b> — кроссплатформенный редактор LaTeX на Qt 6.<br>"
                          "Учебный проект."));
}

bool MainWindow::loadFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("VoidTeX"),
                             tr("Не удалось открыть файл:\n%1\n\n%2")
                                 .arg(path, file.errorString()));
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    m_editor->setPlainText(in.readAll());
    m_editor->document()->setModified(false);

    m_currentFilePath = path;
    addToRecentFiles(path);
    updateWindowTitle();
    statusBar()->showMessage(tr("Открыт: %1").arg(path), 3000);
    return true;
}

bool MainWindow::saveToDisk(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QMessageBox::warning(this, tr("VoidTeX"),
                             tr("Не удалось сохранить файл:\n%1\n\n%2")
                                 .arg(path, file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << m_editor->toPlainText();

    m_currentFilePath = path;
    m_editor->document()->setModified(false);
    addToRecentFiles(path);
    updateWindowTitle();
    statusBar()->showMessage(tr("Сохранено: %1").arg(path), 3000);
    return true;
}

bool MainWindow::maybeSave()
{
    if (!m_editor->document()->isModified())
        return true;

    const auto ret = QMessageBox::warning(this, tr("VoidTeX"),
                                          tr("Документ «%1» был изменён.\nСохранить изменения?")
                                              .arg(m_currentFilePath.isEmpty()
                                                       ? tr("Без имени")
                                                       : QFileInfo(m_currentFilePath).fileName()),
                                          QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    switch (ret) {
    case QMessageBox::Save:    return onSave(), !m_editor->document()->isModified();
    case QMessageBox::Discard: return true;
    default:                   return false;
    }
}

void MainWindow::onClose()
{
    if (maybeSave())
        close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}

void MainWindow::onOpenRecent()
{
    auto *action = qobject_cast<QAction*>(sender());
    if (!action) return;

    const QString path = action->data().toString();
    if (!QFileInfo::exists(path)) {
        QMessageBox::warning(this, tr("VoidTeX"),
                             tr("Файл больше не существует:\n%1").arg(path));
        m_recentFiles.removeAll(path);
        QSettings().setValue("files/recent", m_recentFiles);
        updateRecentFilesMenu();
        return;
    }

    if (!maybeSave()) return;
    loadFile(path);
}

void MainWindow::onDocumentModifiedChanged(bool modified)
{
    Q_UNUSED(modified);
    updateWindowTitle();
}

void MainWindow::addToRecentFiles(const QString &path)
{
    m_recentFiles.removeAll(path);
    m_recentFiles.prepend(path);
    while (m_recentFiles.size() > MaxRecentFiles)
        m_recentFiles.removeLast();

    QSettings().setValue("files/recent", m_recentFiles);
    updateRecentFilesMenu();
}

void MainWindow::updateRecentFilesMenu()
{
    if (!m_recentFilesMenu) return;

    m_recentFilesMenu->clear();

    if (m_recentFiles.isEmpty()) {
        QAction *empty = m_recentFilesMenu->addAction(tr("(нет)"));
        empty->setEnabled(false);
        return;
    }

    for (const QString &path : std::as_const(m_recentFiles)) {
        const QString label = QFileInfo(path).fileName();
        QAction *action = m_recentFilesMenu->addAction(label);
        action->setData(path);
        action->setStatusTip(path);
        action->setToolTip(path);
        connect(action, &QAction::triggered, this, &MainWindow::onOpenRecent);
    }

    m_recentFilesMenu->addSeparator();
    QAction *clear = m_recentFilesMenu->addAction(tr("Очистить список"));
    connect(clear, &QAction::triggered, this, [this]() {
        m_recentFiles.clear();
        QSettings().setValue("files/recent", m_recentFiles);
        updateRecentFilesMenu();
    });
}

void MainWindow::updateWindowTitle()
{
    QString name = m_currentFilePath.isEmpty()
    ? tr("Без имени")
    : QFileInfo(m_currentFilePath).fileName();

    const QString modified = m_editor->document()->isModified() ? "*" : "";
    setWindowTitle(tr("%1%2 — VoidTeX").arg(modified, name));
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const auto urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;

    const QString path = urls.first().toLocalFile();
    if (path.isEmpty()) return;

    if (!maybeSave()) return;
    loadFile(path);
}