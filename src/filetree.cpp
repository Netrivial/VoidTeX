#include "filetree.h"
#include <QFileSystemModel>
#include <QTreeView>
#include <QVBoxLayout>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>

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
    QString selectedFile;
    bool isDir = false;

    if (index.isValid()) {
        currentDir = m_model->filePath(index);
        isDir = m_model->isDir(index);
        if (!isDir) {
            selectedFile = currentDir;
            currentDir = QFileInfo(currentDir).absolutePath();
        }
    } else {
        currentDir = m_model->filePath(m_view->rootIndex());
    }

    QMenu menu(this);

    if (isDir || !index.isValid()) {
        QAction *actNewTex = menu.addAction(tr("Создать файл .tex"));
        QAction *actNewBib = menu.addAction(tr("Создать файл .bib"));
        QAction *actNewDir = menu.addAction(tr("Создать папку"));
        menu.addSeparator();

        QAction *selected = menu.exec(m_view->viewport()->mapToGlobal(pos));

        if (selected == actNewTex || selected == actNewBib || selected == actNewDir) {
            bool ok;
            QString name;

            if (selected == actNewDir) {
                name = QInputDialog::getText(this, tr("Новая папка"), tr("Имя папки:"), QLineEdit::Normal, "", &ok);
                if (ok && !name.isEmpty()) {
                    QDir(currentDir).mkdir(name);
                }
            } else {
                QString filter = (selected == actNewTex) ? ".tex" : ".bib";
                name = QInputDialog::getText(this, tr("Новый файл"), tr("Имя файла:"), QLineEdit::Normal, "untitled" + filter, &ok);
                if (ok && !name.isEmpty()) {
                    if (!name.endsWith(filter)) name += filter;
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
        }
    } else {
        QAction *actSetRoot = menu.addAction(tr("Сделать корневым файлом"));
        menu.addSeparator();
        QAction *actRename = menu.addAction(tr("Переименовать"));
        QAction *actDelete = menu.addAction(tr("Удалить"));

        QAction *selected = menu.exec(m_view->viewport()->mapToGlobal(pos));

        if (selected == actSetRoot) {
            emit setAsRootRequested(selectedFile);
        }
        else if (selected == actRename) {
            bool ok;
            QString newName = QInputDialog::getText(this, tr("Переименовать"), tr("Новое имя:"), QLineEdit::Normal, QFileInfo(selectedFile).fileName(), &ok);
            if (ok && !newName.isEmpty()) {
                QFile::rename(selectedFile, QDir(currentDir).filePath(newName));
            }
        }
        else if (selected == actDelete) {
            auto reply = QMessageBox::question(this, tr("Удалить файл"), tr("Вы уверены, что хотите удалить %1?").arg(QFileInfo(selectedFile).fileName()));
            if (reply == QMessageBox::Yes) {
                QFile::remove(selectedFile);
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
