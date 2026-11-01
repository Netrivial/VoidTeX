#include "filetree.h"

#include <QFileSystemModel>
#include <QTreeView>
#include <QVBoxLayout>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>

FileTree::FileTree(QWidget *parent)
    : QWidget(parent)
{
    m_model = new QFileSystemModel(this);
    m_model->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);

    // m_model->setNameFilters({"*.tex", "*.bib", "*.sty"});
    // m_model->setNameFilterDisables(false);

    m_view = new QTreeView(this);
    m_view->setModel(m_model);
    m_view->setHeaderHidden(true);
    m_view->setAnimated(false);

    for (int i = 1; i < m_model->columnCount(); ++i) {
        m_view->hideColumn(i);
    }

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_view);

    connect(m_view, &QTreeView::doubleClicked, this, &FileTree::onDoubleClicked);

    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_view, &QTreeView::customContextMenuRequested, this, &FileTree::onContextMenuRequested);
}

void FileTree::setRootPath(const QString &path)
{
    if (path.isEmpty()) return;

    m_model->setRootPath(path);
    m_view->setRootIndex(m_model->index(path));
}

void FileTree::onDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;

    if (m_model->isDir(index)) return;

    QString filePath = m_model->filePath(index);
    emit fileDoubleClicked(filePath);
}

void FileTree::onContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = m_view->indexAt(pos);
    QString currentDir;
    QString selectedPath;
    bool isDir = false;

    if (index.isValid()) {
        selectedPath = m_model->filePath(index);
        isDir = m_model->isDir(index);
        currentDir = isDir ? selectedPath : QFileInfo(selectedPath).absolutePath();
    } else {
        currentDir = m_model->filePath(m_view->rootIndex());
    }

    QMenu menu(this);

    // ==========================================
    // ВЕТКА 1: КЛИК ПО ПАПКЕ ИЛИ ПУСТОМУ МЕСТУ
    // ==========================================
    if (isDir || !index.isValid()) {
        QMenu *createMenu = menu.addMenu(tr("Создать"));

        QAction *actQuickTex = createMenu->addAction(tr("Быстро: файл .tex"));
        QAction *actQuickBib = createMenu->addAction(tr("Быстро: файл .bib"));
        createMenu->addSeparator();
        QAction *actNewFile  = createMenu->addAction(tr("Другой файл..."));

        QAction *actNewDir = menu.addAction(tr("Создать папку"));

        QAction *actRenameDir = nullptr;
        QAction *actDeleteDir = nullptr;
        if (isDir) {
            menu.addSeparator();
            actRenameDir = menu.addAction(tr("Переименовать папку"));
            actDeleteDir = menu.addAction(tr("Удалить папку"));
        }

        QAction *selected = menu.exec(m_view->viewport()->mapToGlobal(pos));

        if (!selected) return;

        bool ok;
        QString name;

        // --- Обработка создания файлов ---
        if (selected == actQuickTex || selected == actQuickBib) {
            QString ext = (selected == actQuickTex) ? ".tex" : ".bib";
            name = QInputDialog::getText(this, tr("Новый файл"),
                                         tr("Имя файла (без расширения %1):").arg(ext),
                                         QLineEdit::Normal, "untitled", &ok);
            if (ok && !name.isEmpty()) {
                QString newFilePath = QDir(currentDir).filePath(name + ext);
                QFile f(newFilePath);
                if (f.open(QIODevice::WriteOnly)) {
                    f.close();
                    emit fileCreated(newFilePath);
                } else {
                    QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось создать файл."));
                }
            }
        }
        else if (selected == actNewFile) {
            name = QInputDialog::getText(this, tr("Новый файл"),
                                         tr("Полное имя файла (с расширением):"),
                                         QLineEdit::Normal, "untitled.txt", &ok);
            if (ok && !name.isEmpty()) {
                QString newFilePath = QDir(currentDir).filePath(name);
                QFile f(newFilePath);
                if (f.open(QIODevice::WriteOnly)) {
                    f.close();
                    emit fileCreated(newFilePath);
                } else {
                    QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось создать файл."));
                }
            }
        }
        // --- Обработка создания папки ---
        else if (selected == actNewDir) {
            name = QInputDialog::getText(this, tr("Новая папка"), tr("Имя папки:"), QLineEdit::Normal, "", &ok);
            if (ok && !name.isEmpty()) {
                QDir(currentDir).mkdir(name);
            }
        }
        // --- Обработка переименования папки ---
        else if (selected == actRenameDir) {
            QString newName = QInputDialog::getText(this, tr("Переименовать"),
                                                    tr("Новое имя:"), QLineEdit::Normal,
                                                    QFileInfo(selectedPath).fileName(), &ok);
            if (ok && !newName.isEmpty()) {
                QString parentDir = QFileInfo(selectedPath).absolutePath();
                QString newPath = QDir(parentDir).filePath(newName);

                if (QDir().rename(selectedPath, newPath)) {
                    emit fileRenamed(selectedPath, newPath);
                }
            }
        }
        // --- Обработка удаления папки ---
        else if (selected == actDeleteDir) {
            auto reply = QMessageBox::question(this, tr("Удалить"),
                                               tr("Удалить папку %1 и всё её содержимое?").arg(QFileInfo(selectedPath).fileName()));
            if (reply == QMessageBox::Yes) {
                QDir(selectedPath).removeRecursively();
                emit fileDeleted(selectedPath);
            }
        }
    }
    // ==========================================
    // ВЕТКА 2: КЛИК ПО ФАЙЛУ
    // ==========================================
    else {
        QAction *actSetRoot = menu.addAction(tr("Сделать корневым файлом"));
        menu.addSeparator();
        QAction *actRename = menu.addAction(tr("Переименовать"));
        QAction *actDelete = menu.addAction(tr("Удалить файл"));

        QAction *selected = menu.exec(m_view->viewport()->mapToGlobal(pos));

        if (!selected) return;

        if (selected == actSetRoot) {
            emit setAsRootRequested(selectedPath);
        }
        else if (selected == actRename) {
            QFileInfo fi(selectedPath);
            bool ok;
            QString newBaseName = QInputDialog::getText(this, tr("Переименовать"),
                                                        tr("Новое имя (без расширения .%1):").arg(fi.suffix()),
                                                        QLineEdit::Normal, fi.completeBaseName(), &ok);
            if (ok && !newBaseName.isEmpty()) {
                QString newPath = QDir(currentDir).filePath(newBaseName + "." + fi.suffix());
                if (QFile::rename(selectedPath, newPath)) {
                    emit fileRenamed(selectedPath, newPath);
                }
            }
        }
        else if (selected == actDelete) {
            auto reply = QMessageBox::question(this, tr("Удалить файл"),
                                               tr("Вы уверены, что хотите удалить %1?").arg(QFileInfo(selectedPath).fileName()));
            if (reply == QMessageBox::Yes) {
                QFile::remove(selectedPath);
                emit fileDeleted(selectedPath);
            }
        }
    }
}

void FileTree::setRootFile(const QString &path)
{
    Q_UNUSED(path);
}

void FileTree::clear()
{
    m_view->setRootIndex(QModelIndex());
}
