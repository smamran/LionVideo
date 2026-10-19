#include "MainWindow.h"
#include "../utils/IconHelper.h"
#include "../utils/StyleManager.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QKeyEvent>
#include <QApplication>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QMenu>
#include <QAction>

#if defined(Q_OS_WIN)
#include <windows.h>
#include <windowsx.h>
#endif

namespace ui {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_engine(new core::MediaPlayerEngine(this)) {
    // Window settings
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setWindowIcon(utils::IconHelper::getAppIcon());
    setWindowTitle("Lion Media Player");
    setMinimumSize(640, 420);
    resize(1024, 620);
    setStyleSheet(utils::StyleManager::getApplicationStyleSheet());

    QSettings settings("LionVideo", "LionPlayer");
    m_lastOpenDir = settings.value("lastOpenDir").toString();

    setupUI();
    setupShortcuts();

    qApp->installEventFilter(this);
}

void MainWindow::setupUI() {
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto *rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // 1. Custom Title Bar
    m_titleBar = new TitleBar(this);
    connect(m_titleBar, &TitleBar::minimizeClicked, this, &MainWindow::showMinimized);
    connect(m_titleBar, &TitleBar::maximizeClicked, this, [this]() {
        if (isMaximized()) {
            showNormal();
            m_titleBar->updateWindowState(false);
        } else {
            showMaximized();
            m_titleBar->updateWindowState(true);
        }
    });
    connect(m_titleBar, &TitleBar::closeClicked, this, &MainWindow::close);
    connect(m_titleBar, &TitleBar::pipClicked, this, &MainWindow::togglePiP);
    rootLayout->addWidget(m_titleBar);

    // 2. Video area (central)
    m_videoWidget = new VideoWidget(this);
    m_engine->setVideoOutput(m_videoWidget->videoItem());
    rootLayout->addWidget(m_videoWidget, 1);

    // 3. Floating Overlays (children of m_videoWidget->viewport())
    // Control Bar
    m_controlBar = new ControlBar(m_videoWidget->viewport());
    m_controlBarOpacity = new QGraphicsOpacityEffect(m_controlBar);
    m_controlBar->setGraphicsEffect(m_controlBarOpacity);
    m_controlBarOpacity->setOpacity(1.0);

    m_controlBarAnimation = new QPropertyAnimation(m_controlBarOpacity, "opacity", this);
    connect(m_controlBarAnimation, &QPropertyAnimation::finished, this, [this]() {
        if (m_controlBarOpacity->opacity() <= 0.01) {
            m_controlBar->hide();
        }
    });

    // Playlist Drawer
    m_playlistDrawer = new PlaylistDrawer(m_videoWidget->viewport());
    m_playlistDrawer->hide();

    // OSD Notification Toast
    m_osdLabel = new OSDLabel(m_videoWidget->viewport());

    // Idle timer for auto-hiding controls (2.5 seconds)
    m_idleTimer = new QTimer(this);
    m_idleTimer->setInterval(2500);
    connect(m_idleTimer, &QTimer::timeout, this, &MainWindow::onIdleTimeout);
    m_idleTimer->start();

    // Signal connections: VideoWidget
    connect(m_videoWidget, &VideoWidget::doubleClicked, this, &MainWindow::toggleFullScreen);
    connect(m_videoWidget, &VideoWidget::singleClicked, this, &MainWindow::togglePlay);
    connect(m_videoWidget, &VideoWidget::mouseMoved, this, &MainWindow::onUserInteraction);
    connect(m_videoWidget, &VideoWidget::mouseNearBottom, this, &MainWindow::showControls);
    connect(m_videoWidget, &VideoWidget::widgetResized, this, &MainWindow::repositionOverlays);
    connect(m_videoWidget, &VideoWidget::wheelScrolled, this, [this](int delta) {
        adjustVolume(delta > 0 ? 5 : -5);
    });
    connect(m_videoWidget, &VideoWidget::filesDropped, this, [this](const QList<QUrl> &urls) {
        loadMediaFiles(urls, /*addToQueue=*/false);
    });
    connect(m_videoWidget, &VideoWidget::openFileRequested, this, [this]() {
        openFiles(/*addToQueue=*/false);
    });
    connect(m_videoWidget, &VideoWidget::contextMenuRequested, this, &MainWindow::showContextMenu);

    // Signal connections: ControlBar
    connect(m_controlBar, &ControlBar::playToggled, m_engine, &core::MediaPlayerEngine::togglePlay);
    connect(m_controlBar, &ControlBar::nextClicked, m_engine, &core::MediaPlayerEngine::next);
    connect(m_controlBar, &ControlBar::prevClicked, m_engine, &core::MediaPlayerEngine::previous);
    connect(m_controlBar, &ControlBar::seekRequested, m_engine, &core::MediaPlayerEngine::seek);
    connect(m_controlBar, &ControlBar::volumeChanged, m_engine, &core::MediaPlayerEngine::setVolume);
    connect(m_controlBar, &ControlBar::muteToggled, m_engine, &core::MediaPlayerEngine::toggleMute);
    connect(m_controlBar, &ControlBar::speedChanged, m_engine, &core::MediaPlayerEngine::setPlaybackRate);
    connect(m_controlBar, &ControlBar::loopToggled, this, &MainWindow::toggleLoop);
    connect(m_controlBar, &ControlBar::shuffleToggled, this, &MainWindow::toggleShuffle);
    connect(m_controlBar, &ControlBar::pipToggled, this, &MainWindow::togglePiP);
    connect(m_controlBar, &ControlBar::playlistToggled, this, &MainWindow::togglePlaylistDrawer);
    connect(m_controlBar, &ControlBar::fullscreenToggled, this, &MainWindow::toggleFullScreen);
    connect(m_controlBar, &ControlBar::mouseHoverStateChanged, this, &MainWindow::onControlBarHoverChanged);

    // Signal connections: Engine to ControlBar & UI
    connect(m_engine, &core::MediaPlayerEngine::positionChanged, m_controlBar, &ControlBar::setPosition);
    connect(m_engine, &core::MediaPlayerEngine::durationChanged, m_controlBar, &ControlBar::setDuration);
    connect(m_engine, &core::MediaPlayerEngine::playbackStateChanged, this, &MainWindow::onPlaybackStateChanged);
    connect(m_engine, &core::MediaPlayerEngine::volumeChanged, m_controlBar, &ControlBar::setVolume);
    connect(m_engine, &core::MediaPlayerEngine::mutedChanged, m_controlBar, &ControlBar::setMuted);
    connect(m_engine, &core::MediaPlayerEngine::playbackRateChanged, this, [this](qreal rate) {
        m_controlBar->setPlaybackRate(rate);
        m_osdLabel->showNotification(QString("Speed: %1x").arg(rate), utils::IconHelper::speedIcon());
    });
    connect(m_engine, &core::MediaPlayerEngine::loopModeChanged, m_controlBar, &ControlBar::setLoopMode);
    connect(m_engine, &core::MediaPlayerEngine::shuffleChanged, m_controlBar, &ControlBar::setShuffle);
    connect(m_engine, &core::MediaPlayerEngine::currentTrackChanged, this, &MainWindow::onCurrentTrackChanged);
    connect(m_engine, &core::MediaPlayerEngine::playlistUpdated, this, [this]() {
        m_playlistDrawer->updatePlaylist(m_engine->playlist(), m_engine->currentIndex());
        m_videoWidget->showPlaceholder(m_engine->playlist().isEmpty());
    });
    connect(m_engine, &core::MediaPlayerEngine::mediaError, this, [this](const QString &err) {
        m_osdLabel->showNotification(QString("Error: %1").arg(err));
    });

    // Signal connections: PlaylistDrawer
    connect(m_playlistDrawer, &PlaylistDrawer::itemSelected, m_engine, &core::MediaPlayerEngine::playIndex);
    connect(m_playlistDrawer, &PlaylistDrawer::itemRemoved, m_engine, &core::MediaPlayerEngine::removeMedia);
    connect(m_playlistDrawer, &PlaylistDrawer::playlistCleared, m_engine, &core::MediaPlayerEngine::clearPlaylist);
    connect(m_playlistDrawer, &PlaylistDrawer::addFilesRequested, this, [this]() {
        openFiles(/*addToQueue=*/true);
    });
    connect(m_playlistDrawer, &PlaylistDrawer::closeRequested, this, &MainWindow::togglePlaylistDrawer);
    connect(m_playlistDrawer, &PlaylistDrawer::filesDropped, this, [this](const QList<QUrl> &urls) {
        loadMediaFiles(urls, /*addToQueue=*/true);
    });

    // Initial state
    m_controlBar->setVolume(m_engine->volume());
    m_controlBar->setMuted(m_engine->isMuted());
    m_controlBar->setPlaybackRate(m_engine->playbackRate());
    m_controlBar->setLoopMode(m_engine->loopMode());
    m_controlBar->setShuffle(m_engine->isShuffle());

    repositionOverlays();
}

void MainWindow::setupShortcuts() {
    // Shortcuts are also handled globally in keyPressEvent / eventFilter for maximum responsiveness
}

void MainWindow::repositionOverlays() {
    if (!m_videoWidget || !m_controlBar) return;

    QWidget *vp = m_videoWidget->viewport();
    int vWidth = vp ? vp->width() : m_videoWidget->width();
    int vHeight = vp ? vp->height() : m_videoWidget->height();
    if (vWidth <= 0 || vHeight <= 0) return;

    // Reposition ControlBar (floating near bottom)
    int maxBarWidth = m_isPiP ? (vWidth - 16) : 840;
    int barWidth = std::clamp(vWidth - (m_isPiP ? 16 : 48), 280, maxBarWidth);
    int barHeight = m_controlBar->sizeHint().height();
    int barX = (vWidth - barWidth) / 2;
    int barY = vHeight - barHeight - (m_isPiP ? 8 : 20);

    m_controlBar->setGeometry(barX, barY, barWidth, barHeight);
    m_controlBar->raise();

    // Reposition PlaylistDrawer (right drawer)
    if (m_isDrawerOpen) {
        int drawerWidth = 300;
        m_playlistDrawer->setGeometry(vWidth - drawerWidth, 0, drawerWidth, vHeight);
        m_playlistDrawer->raise();
    }

    // Reposition OSDLabel (top right)
    m_osdLabel->reposition(vp ? vp->size() : m_videoWidget->size());
    m_osdLabel->raise();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    repositionOverlays();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    m_engine->stop();
    QMainWindow::closeEvent(event);
}

void MainWindow::onUserInteraction() {
    showControls();
    unsetCursor();
    m_idleTimer->start();
}

void MainWindow::onIdleTimeout() {
    if (m_engine->playbackState() == QMediaPlayer::PlayingState &&
        !m_isControlBarHovered &&
        !m_isDrawerOpen) {
        // Do not auto-hide if mouse cursor is currently resting near bottom controls
        if (m_videoWidget && m_videoWidget->viewport()) {
            QPoint localPos = m_videoWidget->viewport()->mapFromGlobal(QCursor::pos());
            if (m_videoWidget->viewport()->rect().contains(localPos) && localPos.y() >= (m_videoWidget->viewport()->height() - 120)) {
                m_idleTimer->start();
                return;
            }
        }
        hideControls();
        if (isFullScreen()) {
            setCursor(Qt::BlankCursor);
        }
    }
}

void MainWindow::onControlBarHoverChanged(bool isHovered) {
    m_isControlBarHovered = isHovered;
    if (isHovered) {
        m_idleTimer->stop();
        showControls();
    } else {
        m_idleTimer->start();
    }
}

void MainWindow::showControls() {
    repositionOverlays();
    m_controlBar->show();
    m_controlBar->raise();
    if (m_controlBarOpacity->opacity() >= 0.99 && m_controlBar->isVisible()) {
        return;
    }
    m_controlBarAnimation->stop();
    m_controlBarAnimation->setDuration(120);
    m_controlBarAnimation->setStartValue(m_controlBarOpacity->opacity());
    m_controlBarAnimation->setEndValue(1.0);
    m_controlBarAnimation->start();
}

void MainWindow::hideControls() {
    if (m_controlBarOpacity->opacity() <= 0.01 && !m_controlBar->isVisible()) {
        return;
    }
    m_controlBarAnimation->stop();
    m_controlBarAnimation->setDuration(250);
    m_controlBarAnimation->setStartValue(m_controlBarOpacity->opacity());
    m_controlBarAnimation->setEndValue(0.0);
    m_controlBarAnimation->start();
}

void MainWindow::openFiles(bool addToQueue) {
    QString startDir = m_lastOpenDir;
    if (startDir.isEmpty() || !QDir(startDir).exists()) {
        QSettings settings("LionVideo", "LionPlayer");
        startDir = settings.value("lastOpenDir").toString();
    }
    if (startDir.isEmpty() || !QDir(startDir).exists()) {
        startDir = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    }
    if (startDir.isEmpty() || !QDir(startDir).exists()) {
        startDir = QDir::homePath();
    }

    QString dialogTitle = addToQueue ? "Add Media to Queue" : "Open Media File";
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        dialogTitle,
        startDir,
        "Media Files (*.mp4 *.mkv *.avi *.mp3 *.wav *.webm *.flv *.mov *.m4v *.aac *.ogg *.flac);;All Files (*.*)"
    );

    if (!files.isEmpty()) {
        m_lastOpenDir = QFileInfo(files.first()).absolutePath();
        QSettings settings("LionVideo", "LionPlayer");
        settings.setValue("lastOpenDir", m_lastOpenDir);

        QList<QUrl> urls;
        urls.reserve(files.size());
        for (const auto &f : files) {
            urls.append(QUrl::fromLocalFile(f));
        }
        loadMediaFiles(urls, addToQueue);
    }
}

void MainWindow::togglePlay() {
    m_engine->togglePlay();
    onUserInteraction();
}

void MainWindow::toggleFullScreen() {
    if (m_isPiP) {
        togglePiP(); // Exit PiP before entering Fullscreen
    }

    if (isFullScreen()) {
        showNormal();
        m_titleBar->show();
        m_titleBar->updateWindowState(false);
        m_controlBar->setFullscreen(false);
        unsetCursor();
    } else {
        m_titleBar->hide();
        showFullScreen();
        m_controlBar->setFullscreen(true);
    }
    m_videoWidget->updateVideoBounds();
    repositionOverlays();
    onUserInteraction();
}

void MainWindow::togglePiP() {
    if (isFullScreen()) {
        toggleFullScreen();
    }

    m_isPiP = !m_isPiP;
    m_videoWidget->setPiPMode(m_isPiP);
    m_controlBar->setPiPMode(m_isPiP);

    if (m_isPiP) {
        m_prePiPGeometry = geometry();
        if (m_isDrawerOpen) {
            togglePlaylistDrawer();
        }
        m_titleBar->hide();
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setMinimumSize(320, 180);
        resize(420, 236);
        show();
        m_osdLabel->showNotification("Picture-in-Picture: On", utils::IconHelper::pipIcon());
    } else {
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        setMinimumSize(640, 420);
        if (m_prePiPGeometry.isValid()) {
            setGeometry(m_prePiPGeometry);
        }
        m_titleBar->show();
        show();
        m_osdLabel->showNotification("Picture-in-Picture: Off", utils::IconHelper::pipIcon());
    }

    m_videoWidget->updateVideoBounds();
    repositionOverlays();
}

void MainWindow::togglePlaylistDrawer() {
    if (m_isPiP) return;

    m_isDrawerOpen = !m_isDrawerOpen;
    if (m_isDrawerOpen) {
        m_playlistDrawer->show();
        m_playlistDrawer->raise();
        repositionOverlays();
    } else {
        m_playlistDrawer->hide();
    }
    onUserInteraction();
}

void MainWindow::toggleLoop() {
    m_engine->cycleLoopMode();
    switch (m_engine->loopMode()) {
    case core::LoopMode::None:
        m_osdLabel->showNotification("Loop: Off", utils::IconHelper::loopIcon());
        break;
    case core::LoopMode::All:
        m_osdLabel->showNotification("Loop: All", utils::IconHelper::loopIcon());
        break;
    case core::LoopMode::One:
        m_osdLabel->showNotification("Loop: Current", utils::IconHelper::loopOneIcon());
        break;
    }
    onUserInteraction();
}

void MainWindow::toggleShuffle() {
    m_engine->toggleShuffle();
    if (m_engine->isShuffle()) {
        m_osdLabel->showNotification("Shuffle: On", utils::IconHelper::shuffleIcon());
    } else {
        m_osdLabel->showNotification("Shuffle: Off", utils::IconHelper::shuffleIcon());
    }
    onUserInteraction();
}

void MainWindow::toggleMute() {
    m_engine->toggleMute();
    if (m_engine->isMuted()) {
        m_osdLabel->showNotification("Muted", utils::IconHelper::volumeMutedIcon());
    } else {
        m_osdLabel->showNotification(QString("Volume: %1%").arg(m_engine->volume()), utils::IconHelper::volumeHighIcon());
    }
    onUserInteraction();
}

void MainWindow::adjustVolume(int deltaPercent) {
    int newVol = std::clamp(m_engine->volume() + deltaPercent, 0, 100);
    m_engine->setVolume(newVol);
    m_osdLabel->showNotification(QString("Volume: %1%").arg(newVol),
        (newVol > 50) ? utils::IconHelper::volumeHighIcon() : utils::IconHelper::volumeLowIcon());
    onUserInteraction();
}

void MainWindow::seekRelative(qint64 deltaMs) {
    m_engine->seekRelative(deltaMs);
    QString sign = (deltaMs > 0) ? "+" : "";
    m_osdLabel->showNotification(QString("Seek: %1%2s").arg(sign).arg(deltaMs / 1000));
    onUserInteraction();
}

void MainWindow::adjustSpeed(qreal deltaRate) {
    qreal currentRate = m_engine->playbackRate();
    qreal newRate = std::clamp(currentRate + deltaRate, 0.5, 2.0);
    m_engine->setPlaybackRate(newRate);
    onUserInteraction();
}

void MainWindow::loadMediaFiles(const QList<QUrl> &urls, bool addToQueue) {
    if (urls.isEmpty()) return;

    for (const auto &url : urls) {
        if (url.isLocalFile()) {
            m_lastOpenDir = QFileInfo(url.toLocalFile()).absolutePath();
            QSettings settings("LionVideo", "LionPlayer");
            settings.setValue("lastOpenDir", m_lastOpenDir);
            break;
        }
    }

    if (!addToQueue) {
        // Normal open: replace queue and automatically play
        m_engine->clearPlaylist();
        m_engine->addMediaList(urls, /*autoPlayIfEmpty=*/true);
        if (!m_engine->playlist().isEmpty()) {
            m_engine->playIndex(0);
        }
        m_videoWidget->showPlaceholder(false);
        showControls();
    } else {
        // Explicitly adding to queue (e.g. '+' button, Ctrl+O, or dropping on drawer)
        int oldCount = m_engine->playlist().size();
        int added = m_engine->addMediaList(urls, /*autoPlayIfEmpty=*/oldCount == 0);
        if (oldCount > 0) {
            if (added > 0) {
                m_osdLabel->showNotification(QString("Added %1 file(s) to Queue").arg(added), utils::IconHelper::playlistIcon());
            } else {
                m_osdLabel->showNotification("File(s) already in Queue", utils::IconHelper::playlistIcon());
            }
        }
        onUserInteraction();
    }
}

void MainWindow::onFilesDropped(const QList<QUrl> &urls) {
    loadMediaFiles(urls, /*addToQueue=*/false);
}

void MainWindow::onCurrentTrackChanged(int index, const core::PlaylistItem &item) {
    Q_UNUSED(index);
    m_titleBar->setTitle(item.title);
    m_osdLabel->showNotification(item.title, utils::IconHelper::playIcon(), 2500);
    m_videoWidget->showPlaceholder(false);
    showControls();
}

void MainWindow::onPlaybackStateChanged(QMediaPlayer::PlaybackState state) {
    m_controlBar->setPlaybackState(state);
    if (state == QMediaPlayer::PlayingState) {
        m_videoWidget->showPlaceholder(false);
        showControls();
    } else if (state == QMediaPlayer::StoppedState && m_engine->playlist().isEmpty()) {
        m_videoWidget->showPlaceholder(true);
        m_titleBar->setTitle("");
        repositionOverlays();
    } else {
        onUserInteraction();
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseMove) {
        onUserInteraction();
    } else if (event->type() == QEvent::KeyPress) {
        if (!QApplication::activeModalWidget() && !QApplication::activePopupWidget()) {
            auto *keyEvent = static_cast<QKeyEvent *>(event);
            if (handleKeyPress(keyEvent)) {
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

bool MainWindow::handleKeyPress(QKeyEvent *event) {
    onUserInteraction();

    switch (event->key()) {
    case Qt::Key_Space:
        if (!event->isAutoRepeat()) {
            togglePlay();
        }
        return true;
    case Qt::Key_Left:
        seekRelative(event->modifiers() & Qt::ShiftModifier ? -15000 : -5000);
        return true;
    case Qt::Key_Right:
        seekRelative(event->modifiers() & Qt::ShiftModifier ? 15000 : 5000);
        return true;
    case Qt::Key_Up:
        adjustVolume(5);
        return true;
    case Qt::Key_Down:
        adjustVolume(-5);
        return true;
    case Qt::Key_M:
        if (!event->isAutoRepeat()) {
            toggleMute();
        }
        return true;
    case Qt::Key_L:
        if (!event->isAutoRepeat()) {
            toggleLoop();
        }
        return true;
    case Qt::Key_S:
        if (!event->isAutoRepeat() && !(event->modifiers() & Qt::ControlModifier)) {
            toggleShuffle();
            return true;
        }
        break;
    case Qt::Key_F:
        if (!event->isAutoRepeat()) {
            toggleFullScreen();
        }
        return true;
    case Qt::Key_Escape:
        if (isFullScreen()) {
            toggleFullScreen();
            return true;
        }
        if (m_isPiP) {
            togglePiP();
            return true;
        }
        if (m_isDrawerOpen) {
            togglePlaylistDrawer();
            return true;
        }
        break;
    case Qt::Key_N:
        if (!event->isAutoRepeat()) {
            m_engine->next();
        }
        return true;
    case Qt::Key_P:
        if (!event->isAutoRepeat()) {
            togglePiP();
        }
        return true;
    case Qt::Key_O:
        if (!event->isAutoRepeat()) {
            if (event->modifiers() & Qt::ControlModifier) {
                openFiles(/*addToQueue=*/true);
            } else {
                openFiles(/*addToQueue=*/false);
            }
        }
        return true;
    case Qt::Key_Q:
        if ((event->modifiers() & Qt::ControlModifier) && !event->isAutoRepeat()) {
            togglePlaylistDrawer();
            return true;
        }
        break;
    case Qt::Key_BracketLeft:
        adjustSpeed(-0.25);
        return true;
    case Qt::Key_BracketRight:
        adjustSpeed(0.25);
        return true;
    case Qt::Key_MediaPlay:
    case Qt::Key_MediaPause:
    case Qt::Key_MediaTogglePlayPause:
        if (!event->isAutoRepeat()) {
            togglePlay();
        }
        return true;
    case Qt::Key_MediaNext:
        if (!event->isAutoRepeat()) {
            m_engine->next();
        }
        return true;
    case Qt::Key_MediaPrevious:
    case Qt::Key_PageUp:
        if (!event->isAutoRepeat()) {
            m_engine->previous();
        }
        return true;
    default:
        break;
    }

    return false;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (handleKeyPress(event)) {
        event->accept();
        return;
    }

    QMainWindow::keyPressEvent(event);
}

#if defined(Q_OS_WIN)
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG*>(message);
        if (msg->message == WM_GETMINMAXINFO) {
            auto *mmi = reinterpret_cast<MINMAXINFO *>(msg->lParam);
            HMONITOR hMonitor = MonitorFromWindow(reinterpret_cast<HWND>(winId()), MONITOR_DEFAULTTONEAREST);
            if (hMonitor) {
                MONITORINFO mi;
                mi.cbSize = sizeof(MONITORINFO);
                if (GetMonitorInfo(hMonitor, &mi)) {
                    RECT rcWork = mi.rcWork;
                    RECT rcMonitor = mi.rcMonitor;
                    mmi->ptMaxPosition.x = rcWork.left - rcMonitor.left;
                    mmi->ptMaxPosition.y = rcWork.top - rcMonitor.top;
                    mmi->ptMaxSize.x = rcWork.right - rcWork.left;
                    mmi->ptMaxSize.y = rcWork.bottom - rcWork.top;
                    *result = 0;
                    return true;
                }
            }
        }
        if (msg->message == WM_NCHITTEST && !isFullScreen() && !isMaximized() && !m_isPiP) {
            POINT pt = { GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
            RECT rc;
            GetWindowRect(reinterpret_cast<HWND>(winId()), &rc);
            int border = 8;
            bool left = pt.x < rc.left + border;
            bool right = pt.x >= rc.right - border;
            bool top = pt.y < rc.top + border;
            bool bottom = pt.y >= rc.bottom - border;

            if (top && left) { *result = HTTOPLEFT; return true; }
            if (top && right) { *result = HTTOPRIGHT; return true; }
            if (bottom && left) { *result = HTBOTTOMLEFT; return true; }
            if (bottom && right) { *result = HTBOTTOMRIGHT; return true; }
            if (left) { *result = HTLEFT; return true; }
            if (right) { *result = HTRIGHT; return true; }
            if (top) { *result = HTTOP; return true; }
            if (bottom) { *result = HTBOTTOM; return true; }
        }
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}
#endif

void MainWindow::showContextMenu(const QPoint &globalPos) {
    auto *menu = new QMenu(this);
    menu->setStyleSheet(utils::StyleManager::getMenuDarkStyleSheet());

    auto *openAction = menu->addAction(utils::IconHelper::folderOpenIcon(), "Open File...\tO");
    connect(openAction, &QAction::triggered, this, [this]() {
        openFiles(/*addToQueue=*/false);
    });

    auto *addQueueAction = menu->addAction(utils::IconHelper::plusIcon(), "Add to Queue...\tCtrl+O");
    connect(addQueueAction, &QAction::triggered, this, [this]() {
        openFiles(/*addToQueue=*/true);
    });

    menu->addSeparator();

    bool isPlaying = (m_engine->playbackState() == QMediaPlayer::PlayingState);
    QString playText = isPlaying ? "Pause\tSpace" : "Play\tSpace";
    QIcon playIcon = isPlaying ? utils::IconHelper::pauseIcon() : utils::IconHelper::playIcon();
    auto *playAction = menu->addAction(playIcon, playText);
    connect(playAction, &QAction::triggered, this, &MainWindow::togglePlay);

    auto *pipAction = menu->addAction(utils::IconHelper::pipIcon(), m_isPiP ? "Exit PiP\tP" : "Picture-in-Picture\tP");
    connect(pipAction, &QAction::triggered, this, &MainWindow::togglePiP);

    if (!m_isPiP) {
        auto *fsAction = menu->addAction(utils::IconHelper::fullscreenIcon(), isFullScreen() ? "Exit Fullscreen\tF" : "Fullscreen\tF");
        connect(fsAction, &QAction::triggered, this, &MainWindow::toggleFullScreen);

        auto *queueAction = menu->addAction(utils::IconHelper::playlistIcon(), "Play Queue\tCtrl+Q");
        connect(queueAction, &QAction::triggered, this, &MainWindow::togglePlaylistDrawer);
    }

    menu->exec(globalPos);
    delete menu;
}

} // namespace ui
