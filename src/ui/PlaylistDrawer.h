#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QUrl>
#include <QList>
#include "../core/MediaPlayerEngine.h"

namespace ui {

class PlaylistDrawer : public QWidget {
    Q_OBJECT

public:
    explicit PlaylistDrawer(QWidget *parent = nullptr);
    ~PlaylistDrawer() override = default;

    void updatePlaylist(const QList<core::PlaylistItem> &items, int currentIndex);

signals:
    void itemSelected(int index);
    void itemRemoved(int index);
    void playlistCleared();
    void addFilesRequested();
    void closeRequested();
    void filesDropped(const QList<QUrl> &urls);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void onItemDoubleClicked(QListWidgetItem *item);
    void showContextMenu(const QPoint &pos);

private:
    QLabel *m_titleLabel = nullptr;
    QLabel *m_countLabel = nullptr;
    QPushButton *m_addBtn = nullptr;
    QPushButton *m_clearBtn = nullptr;
    QPushButton *m_closeBtn = nullptr;
    QListWidget *m_listWidget = nullptr;
};

} // namespace ui
