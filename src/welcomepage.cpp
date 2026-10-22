#include "welcomepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QFileInfo>

WelcomePage::WelcomePage(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("WelcomePage");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(60, 60, 60, 60);
    mainLayout->setSpacing(20);

    auto *title = new QLabel("VoidTeX", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(32);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    auto *btnLayout = new QHBoxLayout;
    auto *btnNew = new QPushButton(tr("Создать новый документ"), this);
    auto *btnOpen = new QPushButton(tr("Открыть файл..."), this);
    auto *btnSettings = new QPushButton(tr("Настройки"), this);

    btnLayout->addWidget(btnNew);
    btnLayout->addWidget(btnOpen);
    btnLayout->addWidget(btnSettings);
    mainLayout->addLayout(btnLayout);

    connect(btnNew, &QPushButton::clicked, this, &WelcomePage::newFileRequested);
    connect(btnOpen, &QPushButton::clicked, this, &WelcomePage::openFileRequested);
    connect(btnSettings, &QPushButton::clicked, this, &WelcomePage::settingsRequested);

    auto *recentLabel = new QLabel(tr("Недавние документы:"), this);
    mainLayout->addWidget(recentLabel);

    m_recentList = new QListWidget(this);
    mainLayout->addWidget(m_recentList);

    connect(m_recentList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        emit recentFileRequested(item->data(Qt::UserRole).toString());
    });
}

void WelcomePage::setRecentFiles(const QStringList &files)
{
    m_recentList->clear();
    for (const QString &path : files) {
        auto *item = new QListWidgetItem(QFileInfo(path).fileName(), m_recentList);
        item->setData(Qt::UserRole, path);
        item->setToolTip(path);
    }
}