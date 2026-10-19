#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenu>
#include "ClickableSlider.h"
#include "../core/MediaPlayerEngine.h"

namespace ui {

class ControlBar : public QWidget {
    Q_OBJECT

public:
    explicit ControlBar(QWidget *parent = nullptr);
    ~ControlBar() override = default;

    void setPlaybackState(QMediaPlayer::PlaybackState state);
    void setPosition(qint64 positionMs);
    void setDuration(qint64 durationMs);
    void setVolume(int volumePercent);
    void setMuted(bool isMuted);
    void setPlaybackRate(qreal rate);
    void setLoopMode(core::LoopMode mode);
    void setShuffle(bool isShuffle);
    void setFullscreen(bool isFullscreen);
    void setPiPMode(bool isPiP);

signals:
    void playToggled();
    void nextClicked();
    void prevClicked();
    void seekRequested(qint64 positionMs);
    void volumeChanged(int volumePercent);
    void muteToggled();
    void speedChanged(qreal rate);
    void loopToggled();
    void shuffleToggled();
    void pipToggled();
    void playlistToggled();
    void fullscreenToggled();
    void mouseHoverStateChanged(bool isHovered);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void showSpeedMenu();
    void updateVolumeIcon();

private:
    void setupUI();

    ClickableSlider *m_seekBar = nullptr;
    QLabel *m_timeLabel = nullptr;

    QPushButton *m_playBtn = nullptr;
    QPushButton *m_prevBtn = nullptr;
    QPushButton *m_nextBtn = nullptr;

    QPushButton *m_volumeBtn = nullptr;
    QSlider *m_volumeSlider = nullptr;

    QPushButton *m_speedBtn = nullptr;
    QPushButton *m_loopBtn = nullptr;
    QPushButton *m_shuffleBtn = nullptr;
    QPushButton *m_pipBtn = nullptr;
    QPushButton *m_playlistBtn = nullptr;
    QPushButton *m_fullscreenBtn = nullptr;

    qint64 m_currentPosition = 0;
    qint64 m_currentDuration = 0;
    int m_volume = 70;
    bool m_isMuted = false;
    bool m_isFullscreen = false;
    bool m_isPiP = false;
    bool m_leftPressed = false;
    bool m_isDragging = false;
    QPoint m_dragStartGlobalPos;
    QPoint m_dragStartWindowPos;
};

} // namespace ui
