#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QUrl>
#include <QList>
#include "../core/MediaPlayerEngine.h"
#include "TitleBar.h"
#include "VideoWidget.h"
#include "ControlBar.h"
#include "PlaylistDrawer.h"
#include "OSDLabel.h"

namespace ui {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

public slots:
    void openFiles(bool addToQueue = false);
    void loadMediaFiles(const QList<QUrl> &urls, bool addToQueue = false);
    void togglePlay();
    void toggleFullScreen();
    void togglePiP();
    void togglePlaylistDrawer();
    void toggleLoop();
    void toggleShuffle();
    void toggleMute();
    void adjustVolume(int deltaPercent);
    void seekRelative(qint64 deltaMs);
    void adjustSpeed(qreal deltaRate);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
#if defined(Q_OS_WIN)
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
#endif

private slots:
    void onFilesDropped(const QList<QUrl> &urls);
    void onCurrentTrackChanged(int index, const core::PlaylistItem &item);
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
    void onUserInteraction();
    void onIdleTimeout();
    void onControlBarHoverChanged(bool isHovered);

private:
    void setupUI();
    void setupShortcuts();
    void repositionOverlays();
    void showControls();
    void hideControls();
    bool handleKeyPress(QKeyEvent *event);

    core::MediaPlayerEngine *m_engine = nullptr;

    TitleBar *m_titleBar = nullptr;
    VideoWidget *m_videoWidget = nullptr;
    ControlBar *m_controlBar = nullptr;
    PlaylistDrawer *m_playlistDrawer = nullptr;
    OSDLabel *m_osdLabel = nullptr;

    QGraphicsOpacityEffect *m_controlBarOpacity = nullptr;
    QPropertyAnimation *m_controlBarAnimation = nullptr;
    QTimer *m_idleTimer = nullptr;

    bool m_isPiP = false;
    QRect m_prePiPGeometry;
    bool m_isControlBarHovered = false;
    bool m_isDrawerOpen = false;
    QString m_lastOpenDir;
};

} // namespace ui
