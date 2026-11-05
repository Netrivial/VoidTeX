#include "searchwidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QKeyEvent>

SearchWidget::SearchWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("SearchWidget");
    setMaximumHeight(80);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 4, 8, 4);
    mainLayout->setSpacing(4);

    auto *searchLayout = new QHBoxLayout;
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText(tr("Найти..."));

    m_caseCheck = new QCheckBox(tr("Aa"), this);
    m_caseCheck->setToolTip(tr("Учитывать регистр"));

    m_btnPrev = new QPushButton(tr("Назад"), this);
    m_btnNext = new QPushButton(tr("Вперед"), this);

    auto *btnClose = new QPushButton(tr("✕"), this);
    btnClose->setFixedWidth(30);
    btnClose->setFlat(true);

    searchLayout->addWidget(m_searchEdit);
    searchLayout->addWidget(m_caseCheck);
    searchLayout->addWidget(m_btnPrev);
    searchLayout->addWidget(m_btnNext);
    searchLayout->addWidget(btnClose);
    mainLayout->addLayout(searchLayout);

    auto *replaceLayout = new QHBoxLayout;
    m_replaceEdit = new QLineEdit(this);
    m_replaceEdit->setPlaceholderText(tr("Заменить на..."));
    m_btnReplace = new QPushButton(tr("Заменить"), this);
    m_btnReplaceAll = new QPushButton(tr("Заменить всё"), this);

    replaceLayout->addWidget(m_replaceEdit);
    replaceLayout->addWidget(m_btnReplace);
    replaceLayout->addWidget(m_btnReplaceAll);
    mainLayout->addLayout(replaceLayout);

    connect(btnClose, &QPushButton::clicked, this, [this]() {
        hide();
        emit closed();
    });
    connect(m_btnNext, &QPushButton::clicked, this, [this]() {
        emit findNext(m_searchEdit->text(), true, m_caseCheck->isChecked());
    });
    connect(m_btnPrev, &QPushButton::clicked, this, [this]() {
        emit findNext(m_searchEdit->text(), false, m_caseCheck->isChecked());
    });
    connect(m_searchEdit, &QLineEdit::returnPressed, this, [this]() {
        emit findNext(m_searchEdit->text(), true, m_caseCheck->isChecked());
    });
    connect(m_btnReplace, &QPushButton::clicked, this, [this]() {
        emit replaceCurrent(m_searchEdit->text(), m_replaceEdit->text(), m_caseCheck->isChecked());
    });
    connect(m_btnReplaceAll, &QPushButton::clicked, this, [this]() {
        emit replaceAll(m_searchEdit->text(), m_replaceEdit->text(), m_caseCheck->isChecked());
    });

    hide();
}

void SearchWidget::showAndFocus()
{
    show();
    m_searchEdit->setFocus();
    m_searchEdit->selectAll();
}

void SearchWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        hide();
        emit closed();
    } else {
        QWidget::keyPressEvent(event);
    }
}