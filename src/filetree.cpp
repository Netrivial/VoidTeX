#include "filetree.h"
#include <QFileSystemModel>
#include <QTreeView>
#include <QVBoxLayout>
#include <QMenu>

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
    if (!index.isValid() || m_model->isDir(index)) return;

    QString filePath = m_model->filePath(index);

    QMenu menu(this);
    QAction *setRootAction = menu.addAction(tr("Сделать корневым файлом"));

    QAction *selected = menu.exec(m_view->viewport()->mapToGlobal(pos));
    if (selected == setRootAction) {
        emit setAsRootRequested(filePath);
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
