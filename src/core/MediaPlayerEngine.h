#pragma once

#include <QObject>
#include <QUrl>
#include <QList>
#include <QString>
#include <QMediaPlayer>
#include <QAudioOutput>

namespace core {

struct PlaylistItem {
    QUrl url;
    QString title;
    qint64 durationMs = 0;
};

enum class LoopMode {
    None,
    All,
    One
};

class MediaPlayerEngine : public QObject {
    Q_OBJECT

public:
    explicit MediaPlayerEngine(QObject *parent = nullptr);
    ~MediaPlayerEngine() override = default;

    void setVideoOutput(QObject *videoOutput);

    // Playback control
    void play();
    void pause();
    void togglePlay();
    void stop();
    void next();
    void previous();
    void seek(qint64 positionMs);
    void seekRelative(qint64 deltaMs);

    // Volume & Audio
    void setVolume(int volumePercent);
    [[nodiscard]] int volume() const;
    void setMuted(bool muted);
    [[nodiscard]] bool isMuted() const;
    void toggleMute();

    // Speed / Rate
    void setPlaybackRate(qreal rate);
    [[nodiscard]] qreal playbackRate() const;

    // Loop & Shuffle
    void setLoopMode(LoopMode mode);
    [[nodiscard]] LoopMode loopMode() const;
    void cycleLoopMode();
    void setShuffle(bool enabled);
    [[nodiscard]] bool isShuffle() const;
    void toggleShuffle();

    // Playlist
    bool addMedia(const QUrl &url, bool autoPlayIfEmpty = true);
    int addMediaList(const QList<QUrl> &urls, bool autoPlayIfEmpty = true);
    void playIndex(int index);
    void removeMedia(int index);
    void clearPlaylist();

    [[nodiscard]] const QList<PlaylistItem>& playlist() const { return m_playlist; }
    [[nodiscard]] int currentIndex() const { return m_currentIndex; }
    [[nodiscard]] PlaylistItem currentItem() const;
    [[nodiscard]] qint64 position() const;
    [[nodiscard]] qint64 duration() const;
    [[nodiscard]] QMediaPlayer::PlaybackState playbackState() const;
    [[nodiscard]] bool hasNext() const;
    [[nodiscard]] bool hasPrevious() const;

signals:
    void positionChanged(qint64 positionMs);
    void durationChanged(qint64 durationMs);
    void playbackStateChanged(QMediaPlayer::PlaybackState state);
    void volumeChanged(int volumePercent);
    void mutedChanged(bool isMuted);
    void playbackRateChanged(qreal rate);
    void loopModeChanged(LoopMode mode);
    void shuffleChanged(bool isShuffle);
    void currentTrackChanged(int index, const PlaylistItem &item);
    void playlistUpdated();
    void mediaError(const QString &errorMessage);

private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onDurationChanged(qint64 durationMs);
    void onPlayerError(QMediaPlayer::Error error, const QString &errorString);

private:
    void rebuildShuffleList();
    int getNextIndex() const;
    int getPreviousIndex() const;

    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;

    QList<PlaylistItem> m_playlist;
    QList<int> m_shuffledOrder;
    int m_currentIndex = -1;

    LoopMode m_loopMode = LoopMode::None;
    bool m_isShuffle = false;
    qreal m_playbackRate = 1.0;
    int m_lastVolume = 70;
};

} // namespace core
