#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>

class QLineEdit;
class QCheckBox;
class QPushButton;

class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWidget(QWidget *parent = nullptr);

    void showAndFocus();

signals:
    void findNext(const QString &text, bool forward, bool caseSensitive);
    void replaceCurrent(const QString &text, const QString &replaceWith, bool caseSensitive);
    void replaceAll(const QString &text, const QString &replaceWith, bool caseSensitive);
    void closed();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QLineEdit   *m_searchEdit    = nullptr;
    QLineEdit   *m_replaceEdit   = nullptr;
    QCheckBox   *m_caseCheck     = nullptr;
    QPushButton *m_btnNext       = nullptr;
    QPushButton *m_btnPrev       = nullptr;
    QPushButton *m_btnReplace    = nullptr;
    QPushButton *m_btnReplaceAll = nullptr;
};

#endif // SEARCHWIDGET_H