#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

#include <QWidget>
#include <QStringList>

class QListWidget;

class WelcomePage : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomePage(QWidget *parent = nullptr);

    void setRecentFiles(const QStringList &files);

signals:
    void newFileRequested();
    void openFileRequested();
    void recentFileRequested(const QString &path);
    void settingsRequested();

private:
    QListWidget *m_recentList = nullptr;
};

#endif // WELCOMEPAGE_H