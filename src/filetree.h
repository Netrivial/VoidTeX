#ifndef FILETREE_H
#define FILETREE_H

#include <QWidget>

class QFileSystemModel;
class QTreeView;

class FileTree : public QWidget
{
    Q_OBJECT

public:
    explicit FileTree(QWidget *parent = nullptr);

    void setRootPath(const QString &path);

signals:
    void fileDoubleClicked(const QString &path);

private slots:
    void onDoubleClicked(const QModelIndex &index);

private:
    QFileSystemModel *m_model = nullptr;
    QTreeView        *m_view  = nullptr;
};

#endif // FILETREE_H