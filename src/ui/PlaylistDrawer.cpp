#include "PlaylistDrawer.h"
#include "../utils/IconHelper.h"
#include "../utils/TimeFormatter.h"
#include "../utils/StyleManager.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMenu>
#include <QFileInfo>
#include <QProcess>
#include <QDir>

namespace ui {

PlaylistDrawer::PlaylistDrawer(QWidget *parent)
    : QWidget(parent) {
    setObjectName("PlaylistDrawerWidget");
    setFixedWidth(300);
    setAcceptDrops(true);

    setStyleSheet(
        "#PlaylistDrawerWidget {"
        "  background-color: rgba(24, 24, 28, 0.95);"
        "  border-left: 1px solid rgba(255, 255, 255, 0.10);"
        "}"
        "#PlaylistTitle {"
        "  font-size: 14px;"
        "  font-weight: 600;"
        "  color: #FFFFFF;"
        "}"
        "#PlaylistCount {"
        "  font-size: 11px;"
        "  color: #8E8E93;"
        "}"
        ".DrawerBtn {"
        "  background: transparent;"
        "  border: none;"
        "  border-radius: 4px;"
        "  padding: 4px;"
        "}"
        ".DrawerBtn:hover {"
        "  background-color: rgba(255, 255, 255, 0.08);"
        "}"
        ".DrawerBtn:pressed {"
        "  background-color: rgba(255, 255, 255, 0.14);"
        "}"
        "QListWidget {"
        "  background: transparent;"
        "  border: none;"
        "  outline: none;"
        "  padding: 4px;"
        "}"
        "QListWidget::item {"
        "  background: transparent;"
        "  border-radius: 6px;"
        "  padding: 8px 10px;"
        "  margin-bottom: 2px;"
        "  color: #D8D8DC;"
        "}"
        "QListWidget::item:hover {"
        "  background-color: rgba(255, 255, 255, 0.06);"
        "  color: #FFFFFF;"
        "}"
        "QListWidget::item:selected {"
        "  background-color: rgba(96, 205, 255, 0.18);"
        "  border-left: 3px solid #60CDFF;"
        "  color: #60CDFF;"
        "  font-weight: 600;"
        "}"
    );

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 8, 12);
    mainLayout->setSpacing(8);

    // Header
    auto *headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(6);

    auto *titlesLayout = new QVBoxLayout();
    titlesLayout->setSpacing(1);
    m_titleLabel = new QLabel("Play Queue", this);
    m_titleLabel->setObjectName("PlaylistTitle");
    m_countLabel = new QLabel("0 tracks", this);
    m_countLabel->setObjectName("PlaylistCount");
    titlesLayout->addWidget(m_titleLabel);
    titlesLayout->addWidget(m_countLabel);
    headerLayout->addLayout(titlesLayout);

    headerLayout->addStretch(1);

    m_addBtn = new QPushButton(this);
    m_addBtn->setProperty("class", "DrawerBtn");
    m_addBtn->setIcon(utils::IconHelper::plusIcon());
    m_addBtn->setIconSize(QSize(14, 14));
    m_addBtn->setFixedSize(28, 28);
    m_addBtn->setToolTip("Add Media Files to Queue (Ctrl+O)");
    m_addBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_addBtn, &QPushButton::clicked, this, &PlaylistDrawer::addFilesRequested);
    headerLayout->addWidget(m_addBtn);

    m_clearBtn = new QPushButton(this);
    m_clearBtn->setProperty("class", "DrawerBtn");
    m_clearBtn->setIcon(utils::IconHelper::trashIcon());
    m_clearBtn->setIconSize(QSize(14, 14));
    m_clearBtn->setFixedSize(28, 28);
    m_clearBtn->setToolTip("Clear Queue");
    m_clearBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_clearBtn, &QPushButton::clicked, this, &PlaylistDrawer::playlistCleared);
    headerLayout->addWidget(m_clearBtn);

    m_closeBtn = new QPushButton(this);
    m_closeBtn->setProperty("class", "DrawerBtn");
    m_closeBtn->setIcon(utils::IconHelper::windowCloseIcon());
    m_closeBtn->setIconSize(QSize(12, 12));
    m_closeBtn->setFixedSize(28, 28);
    m_closeBtn->setToolTip("Close Queue (Ctrl+Q)");
    m_closeBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_closeBtn, &QPushButton::clicked, this, &PlaylistDrawer::closeRequested);
    headerLayout->addWidget(m_closeBtn);

    mainLayout->addLayout(headerLayout);

    // Queue List
    m_listWidget = new QListWidget(this);
    m_listWidget->setFocusPolicy(Qt::NoFocus);
    m_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_listWidget, &QListWidget::itemDoubleClicked, this, &PlaylistDrawer::onItemDoubleClicked);
    connect(m_listWidget, &QListWidget::customContextMenuRequested, this, &PlaylistDrawer::showContextMenu);
    mainLayout->addWidget(m_listWidget, 1);
}

void PlaylistDrawer::updatePlaylist(const QList<core::PlaylistItem> &items, int currentIndex) {
    m_listWidget->clear();
    m_countLabel->setText(QString("%1 tracks").arg(items.size()));

    for (int i = 0; i < items.size(); ++i) {
        const auto &item = items.at(i);
        QString durationStr = (item.durationMs > 0)
            ? QString(" (%1)").arg(utils::TimeFormatter::formatTime(item.durationMs))
            : "";
        QString text = QString("%1. %2%3").arg(i + 1, 2, 10, QChar('0')).arg(item.title).arg(durationStr);

        auto *listItem = new QListWidgetItem(text, m_listWidget);
        listItem->setData(Qt::UserRole, i);

        if (i == currentIndex) {
            m_listWidget->setCurrentItem(listItem);
            listItem->setIcon(utils::IconHelper::playIcon());
        }
    }
}

void PlaylistDrawer::onItemDoubleClicked(QListWidgetItem *item) {
    if (!item) return;
    int index = item->data(Qt::UserRole).toInt();
    m_listWidget->clearFocus();
    if (window()) {
        window()->setFocus();
    }
    emit itemSelected(index);
}

void PlaylistDrawer::showContextMenu(const QPoint &pos) {
    QListWidgetItem *item = m_listWidget->itemAt(pos);
    if (!item) return;

    int index = item->data(Qt::UserRole).toInt();

    auto *menu = new QMenu(this);
    menu->setStyleSheet(utils::StyleManager::getMenuDarkStyleSheet());

    QAction *playAct = menu->addAction("Play Track");
    QAction *removeAct = menu->addAction("Remove from Queue");
    menu->addSeparator();
    QAction *openFolderAct = menu->addAction("Show in File Explorer");

    connect(playAct, &QAction::triggered, this, [this, index]() {
        m_listWidget->clearFocus();
        if (window()) {
            window()->setFocus();
        }
        emit itemSelected(index);
    });
    connect(removeAct, &QAction::triggered, this, [this, index]() {
        emit itemRemoved(index);
    });
    connect(openFolderAct, &QAction::triggered, this, [this, index]() {
        // Will be delegated or opened
    });

    menu->exec(m_listWidget->mapToGlobal(pos));
}

void PlaylistDrawer::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void PlaylistDrawer::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void PlaylistDrawer::dropEvent(QDropEvent *event) {
    const QList<QUrl> urls = event->mimeData()->urls();
    if (!urls.isEmpty()) {
        emit filesDropped(urls);
        event->acceptProposedAction();
    }
}

} // namespace ui
