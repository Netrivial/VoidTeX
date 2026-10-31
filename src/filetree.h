#ifndef FILETREE_H
#define FILETREE_H

#include <QWidget>
#include <QMouseEvent>

class QFileSystemModel;
class QTreeView;

class FileTree : public QWidget
{
    Q_OBJECT

public:
    explicit FileTree(QWidget *parent = nullptr);

    void setRootPath(const QString &path);
    void setRootFile(const QString &path);
    void clear();

signals:
    void fileDoubleClicked(const QString &path);
    void setAsRootRequested(const QString &path);
    void fileCreated(const QString &path);

private slots:
    void onDoubleClicked(const QModelIndex &index);
    void onContextMenuRequested(const QPoint &pos);

private:
    QFileSystemModel *m_model = nullptr;
    QTreeView        *m_view  = nullptr;
};

#endif // FILETREE_H