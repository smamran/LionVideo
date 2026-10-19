#include "ControlBar.h"
#include "../utils/IconHelper.h"
#include "../utils/TimeFormatter.h"
#include "../utils/StyleManager.h"
#include <QGraphicsDropShadowEffect>
#include <QActionGroup>

namespace ui {

ControlBar::ControlBar(QWidget *parent)
    : QWidget(parent) {
    setObjectName("ControlBarWidget");
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setMouseTracking(true);
    setupUI();
}

QSize ControlBar::sizeHint() const {
    return QSize(680, 80);
}

QSize ControlBar::minimumSizeHint() const {
    return QSize(280, 76);
}

void ControlBar::setupUI() {
    setStyleSheet(
        "#ControlBarWidget {"
        "  background-color: rgba(28, 28, 32, 0.92);"
        "  border: 1px solid rgba(255, 255, 255, 0.12);"
        "  border-radius: 12px;"
        "}"
        "QLabel {"
        "  color: #CCCCCC;"
        "  font-size: 12px;"
        "  font-weight: 500;"
        "  background: transparent;"
        "}"
        ".CtrlBtn {"
        "  background: transparent;"
        "  border: none;"
        "  border-radius: 6px;"
        "  padding: 4px;"
        "}"
        ".CtrlBtn:hover {"
        "  background-color: rgba(255, 255, 255, 0.08);"
        "}"
        ".CtrlBtn:pressed {"
        "  background-color: rgba(255, 255, 255, 0.14);"
        "}"
        "#PlayBtn {"
        "  background-color: #60CDFF;"
        "  border: none;"
        "  border-radius: 18px;"
        "}"
        "#PlayBtn:hover {"
        "  background-color: #78D5FF;"
        "}"
        "#PlayBtn:pressed {"
        "  background-color: #44B8F0;"
        "}"
        "#SpeedBtn {"
        "  background-color: rgba(255, 255, 255, 0.06);"
        "  border: 1px solid rgba(255, 255, 255, 0.12);"
        "  border-radius: 6px;"
        "  padding: 3px 8px;"
        "  color: #FFFFFF;"
        "  font-size: 11px;"
        "  font-weight: 600;"
        "}"
        "#SpeedBtn:hover {"
        "  background-color: rgba(255, 255, 255, 0.12);"
        "}"
        "QSlider, ClickableSlider {"
        "  background: transparent;"
        "}"
        "QSlider::groove:horizontal {"
        "  height: 4px;"
        "  background: rgba(255, 255, 255, 0.20);"
        "  border-radius: 2px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "  background: #60CDFF;"
        "  border-radius: 2px;"
        "}"
        "QSlider::add-page:horizontal {"
        "  background: rgba(255, 255, 255, 0.20);"
        "  border-radius: 2px;"
        "}"
        "QSlider::handle:horizontal {"
        "  background: #FFFFFF;"
        "  width: 12px;"
        "  height: 12px;"
        "  margin: -4px 0;"
        "  border-radius: 6px;"
        "}"
        "QSlider::handle:horizontal:hover {"
        "  background: #78D5FF;"
        "  width: 14px;"
        "  height: 14px;"
        "  margin: -5px 0;"
        "  border-radius: 7px;"
        "}"
    );

    // Drop shadow
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(24);
    shadow->setColor(QColor(0, 0, 0, 160));
    shadow->setOffset(0, 4);
    setGraphicsEffect(shadow);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(14, 8, 14, 10);
    mainLayout->setSpacing(6);

    // 1. Seekbar
    m_seekBar = new ClickableSlider(Qt::Horizontal, this);
    m_seekBar->setAttribute(Qt::WA_TranslucentBackground, true);
    m_seekBar->setStyleSheet("background: transparent;");
    connect(m_seekBar, &ClickableSlider::seekRequested, this, &ControlBar::seekRequested);
    mainLayout->addWidget(m_seekBar);

    // 2. Control Row
    auto *bottomLayout = new QHBoxLayout();
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->setSpacing(8);

    // Playback buttons group
    m_prevBtn = new QPushButton(this);
    m_prevBtn->setProperty("class", "CtrlBtn");
    m_prevBtn->setIcon(utils::IconHelper::prevIcon());
    m_prevBtn->setIconSize(QSize(16, 16));
    m_prevBtn->setFixedSize(32, 32);
    m_prevBtn->setToolTip("Previous Track (PageUp)");
    m_prevBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_prevBtn, &QPushButton::clicked, this, &ControlBar::prevClicked);

    m_playBtn = new QPushButton(this);
    m_playBtn->setObjectName("PlayBtn");
    m_playBtn->setIcon(utils::IconHelper::playIcon());
    m_playBtn->setIconSize(QSize(18, 18));
    m_playBtn->setFixedSize(36, 36);
    m_playBtn->setToolTip("Play / Pause (Space)");
    m_playBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_playBtn, &QPushButton::clicked, this, &ControlBar::playToggled);

    m_nextBtn = new QPushButton(this);
    m_nextBtn->setProperty("class", "CtrlBtn");
    m_nextBtn->setIcon(utils::IconHelper::nextIcon());
    m_nextBtn->setIconSize(QSize(16, 16));
    m_nextBtn->setFixedSize(32, 32);
    m_nextBtn->setToolTip("Next (N)");
    m_nextBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_nextBtn, &QPushButton::clicked, this, &ControlBar::nextClicked);

    bottomLayout->addWidget(m_prevBtn);
    bottomLayout->addWidget(m_playBtn);
    bottomLayout->addWidget(m_nextBtn);

    // Volume group
    m_volumeBtn = new QPushButton(this);
    m_volumeBtn->setProperty("class", "CtrlBtn");
    m_volumeBtn->setIcon(utils::IconHelper::volumeHighIcon());
    m_volumeBtn->setIconSize(QSize(16, 16));
    m_volumeBtn->setFixedSize(30, 30);
    m_volumeBtn->setToolTip("Mute / Unmute (M)");
    m_volumeBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_volumeBtn, &QPushButton::clicked, this, &ControlBar::muteToggled);

    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setAttribute(Qt::WA_TranslucentBackground, true);
    m_volumeSlider->setStyleSheet("background: transparent;");
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(70);
    m_volumeSlider->setFixedWidth(80);
    m_volumeSlider->setFixedHeight(20);
    m_volumeSlider->setToolTip("Volume (Up/Down)");
    m_volumeSlider->setFocusPolicy(Qt::NoFocus);
    connect(m_volumeSlider, &QSlider::valueChanged, this, &ControlBar::volumeChanged);

    bottomLayout->addWidget(m_volumeBtn);
    bottomLayout->addWidget(m_volumeSlider);

    // Time Label
    m_timeLabel = new QLabel("00:00 / 00:00", this);
    bottomLayout->addWidget(m_timeLabel);

    bottomLayout->addStretch(1);

    // Speed button
    m_speedBtn = new QPushButton("1.0x", this);
    m_speedBtn->setObjectName("SpeedBtn");
    m_speedBtn->setFixedHeight(26);
    m_speedBtn->setToolTip("Playback Speed");
    m_speedBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_speedBtn, &QPushButton::clicked, this, &ControlBar::showSpeedMenu);
    bottomLayout->addWidget(m_speedBtn);

    // Loop button
    m_loopBtn = new QPushButton(this);
    m_loopBtn->setProperty("class", "CtrlBtn");
    m_loopBtn->setIcon(utils::IconHelper::loopIcon());
    m_loopBtn->setIconSize(QSize(16, 16));
    m_loopBtn->setFixedSize(30, 30);
    m_loopBtn->setToolTip("Loop Mode: Off (L)");
    m_loopBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_loopBtn, &QPushButton::clicked, this, &ControlBar::loopToggled);
    bottomLayout->addWidget(m_loopBtn);

    // Shuffle button
    m_shuffleBtn = new QPushButton(this);
    m_shuffleBtn->setProperty("class", "CtrlBtn");
    m_shuffleBtn->setIcon(utils::IconHelper::shuffleIcon());
    m_shuffleBtn->setIconSize(QSize(16, 16));
    m_shuffleBtn->setFixedSize(30, 30);
    m_shuffleBtn->setToolTip("Shuffle: Off (S)");
    m_shuffleBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_shuffleBtn, &QPushButton::clicked, this, &ControlBar::shuffleToggled);
    bottomLayout->addWidget(m_shuffleBtn);

    // PiP Button
    m_pipBtn = new QPushButton(this);
    m_pipBtn->setProperty("class", "CtrlBtn");
    m_pipBtn->setIcon(utils::IconHelper::pipIcon());
    m_pipBtn->setIconSize(QSize(16, 16));
    m_pipBtn->setFixedSize(30, 30);
    m_pipBtn->setToolTip("Picture-in-Picture (P)");
    m_pipBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_pipBtn, &QPushButton::clicked, this, &ControlBar::pipToggled);
    bottomLayout->addWidget(m_pipBtn);

    // Playlist button
    m_playlistBtn = new QPushButton(this);
    m_playlistBtn->setProperty("class", "CtrlBtn");
    m_playlistBtn->setIcon(utils::IconHelper::playlistIcon());
    m_playlistBtn->setIconSize(QSize(16, 16));
    m_playlistBtn->setFixedSize(30, 30);
    m_playlistBtn->setToolTip("Play Queue (Ctrl+Q)");
    m_playlistBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_playlistBtn, &QPushButton::clicked, this, &ControlBar::playlistToggled);
    bottomLayout->addWidget(m_playlistBtn);

    // Fullscreen button
    m_fullscreenBtn = new QPushButton(this);
    m_fullscreenBtn->setProperty("class", "CtrlBtn");
    m_fullscreenBtn->setIcon(utils::IconHelper::fullscreenIcon());
    m_fullscreenBtn->setIconSize(QSize(16, 16));
    m_fullscreenBtn->setFixedSize(30, 30);
    m_fullscreenBtn->setToolTip("Fullscreen (F)");
    m_fullscreenBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_fullscreenBtn, &QPushButton::clicked, this, &ControlBar::fullscreenToggled);
    bottomLayout->addWidget(m_fullscreenBtn);

    mainLayout->addLayout(bottomLayout);
}

void ControlBar::setPlaybackState(QMediaPlayer::PlaybackState state) {
    if (state == QMediaPlayer::PlayingState) {
        m_playBtn->setIcon(utils::IconHelper::pauseIcon());
    } else {
        m_playBtn->setIcon(utils::IconHelper::playIcon());
    }
}

void ControlBar::setPosition(qint64 positionMs) {
    m_currentPosition = positionMs;
    if (!m_seekBar->isSliderDown()) {
        m_seekBar->setValue(positionMs);
    }
    m_timeLabel->setText(utils::TimeFormatter::formatTimeWithTotal(m_currentPosition, m_currentDuration));
}

void ControlBar::setDuration(qint64 durationMs) {
    m_currentDuration = durationMs;
    m_seekBar->setRange(0, durationMs);
    m_timeLabel->setText(utils::TimeFormatter::formatTimeWithTotal(m_currentPosition, m_currentDuration));
}

void ControlBar::setVolume(int volumePercent) {
    m_volume = volumePercent;
    if (m_volumeSlider->value() != volumePercent) {
        m_volumeSlider->blockSignals(true);
        m_volumeSlider->setValue(volumePercent);
        m_volumeSlider->blockSignals(false);
    }
    updateVolumeIcon();
}

void ControlBar::setMuted(bool isMuted) {
    m_isMuted = isMuted;
    updateVolumeIcon();
}

void ControlBar::updateVolumeIcon() {
    if (m_isMuted || m_volume == 0) {
        m_volumeBtn->setIcon(utils::IconHelper::volumeMutedIcon());
        m_volumeBtn->setToolTip("Unmute (M)");
    } else if (m_volume < 33) {
        m_volumeBtn->setIcon(utils::IconHelper::volumeLowIcon());
        m_volumeBtn->setToolTip("Mute (M)");
    } else if (m_volume < 66) {
        m_volumeBtn->setIcon(utils::IconHelper::volumeMedIcon());
        m_volumeBtn->setToolTip("Mute (M)");
    } else {
        m_volumeBtn->setIcon(utils::IconHelper::volumeHighIcon());
        m_volumeBtn->setToolTip("Mute (M)");
    }
}

void ControlBar::setPlaybackRate(qreal rate) {
    m_speedBtn->setText(QString("%1x").arg(rate, 0, 'f', (rate == static_cast<int>(rate)) ? 1 : 2));
}

void ControlBar::setLoopMode(core::LoopMode mode) {
    switch (mode) {
    case core::LoopMode::None:
        m_loopBtn->setIcon(utils::IconHelper::loopIcon());
        m_loopBtn->setStyleSheet("");
        m_loopBtn->setToolTip("Loop: Off (L)");
        break;
    case core::LoopMode::All:
        m_loopBtn->setIcon(utils::IconHelper::loopIcon());
        m_loopBtn->setStyleSheet("background-color: rgba(96, 205, 255, 0.22); border-radius: 6px;");
        m_loopBtn->setToolTip("Loop: All (L)");
        break;
    case core::LoopMode::One:
        m_loopBtn->setIcon(utils::IconHelper::loopOneIcon());
        m_loopBtn->setStyleSheet("background-color: rgba(96, 205, 255, 0.22); border-radius: 6px;");
        m_loopBtn->setToolTip("Loop: Current (L)");
        break;
    }
}

void ControlBar::setShuffle(bool isShuffle) {
    if (isShuffle) {
        m_shuffleBtn->setStyleSheet("background-color: rgba(96, 205, 255, 0.22); border-radius: 6px;");
        m_shuffleBtn->setToolTip("Shuffle: On (S)");
    } else {
        m_shuffleBtn->setStyleSheet("");
        m_shuffleBtn->setToolTip("Shuffle: Off (S)");
    }
}

void ControlBar::setFullscreen(bool isFullscreen) {
    m_isFullscreen = isFullscreen;
    if (m_isFullscreen) {
        m_fullscreenBtn->setIcon(utils::IconHelper::fullscreenExitIcon());
        m_fullscreenBtn->setToolTip("Exit Fullscreen (Esc or F)");
    } else {
        m_fullscreenBtn->setIcon(utils::IconHelper::fullscreenIcon());
        m_fullscreenBtn->setToolTip("Fullscreen (F)");
    }
}

void ControlBar::showSpeedMenu() {
    auto *menu = new QMenu(this);
    menu->setStyleSheet(utils::StyleManager::getMenuDarkStyleSheet());

    const QList<qreal> speeds = {0.5, 0.75, 1.0, 1.25, 1.5, 2.0};
    for (qreal speed : speeds) {
        QString text = (speed == 1.0) ? "1.0x (Normal)" : QString("%1x").arg(speed);
        QAction *action = menu->addAction(text);
        action->setCheckable(true);
        if (m_speedBtn->text().startsWith(QString::number(speed))) {
            action->setChecked(true);
        }
        connect(action, &QAction::triggered, this, [this, speed]() {
            emit speedChanged(speed);
        });
    }

    menu->exec(m_speedBtn->mapToGlobal(QPoint(0, -menu->sizeHint().height())));
}

void ControlBar::setPiPMode(bool isPiP) {
    m_isPiP = isPiP;
    m_leftPressed = false;
    m_isDragging = false;
    unsetCursor();

    m_speedBtn->setVisible(!isPiP);
    m_loopBtn->setVisible(!isPiP);
    m_shuffleBtn->setVisible(!isPiP);
    m_playlistBtn->setVisible(!isPiP);
}

void ControlBar::enterEvent(QEnterEvent *event) {
    emit mouseHoverStateChanged(true);
    QWidget::enterEvent(event);
}

void ControlBar::leaveEvent(QEvent *event) {
    emit mouseHoverStateChanged(false);
    QWidget::leaveEvent(event);
}

void ControlBar::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_isPiP) {
        m_leftPressed = true;
        m_isDragging = false;
        m_dragStartGlobalPos = event->globalPosition().toPoint();
        m_dragStartWindowPos = window()->pos();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void ControlBar::mouseMoveEvent(QMouseEvent *event) {
    emit mouseHoverStateChanged(true);
    if (m_leftPressed && m_isPiP && (event->buttons() & Qt::LeftButton)) {
        QPoint delta = event->globalPosition().toPoint() - m_dragStartGlobalPos;
        if (!m_isDragging && delta.manhattanLength() >= 4) {
            m_isDragging = true;
            setCursor(Qt::ClosedHandCursor);
        }
        if (m_isDragging) {
            window()->move(m_dragStartWindowPos + delta);
            event->accept();
            return;
        }
    }
    QWidget::mouseMoveEvent(event);
}

void ControlBar::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_leftPressed) {
        m_leftPressed = false;
        if (m_isDragging) {
            m_isDragging = false;
            unsetCursor();
            event->accept();
            return;
        }
        unsetCursor();
    }
    QWidget::mouseReleaseEvent(event);
}

} // namespace ui
