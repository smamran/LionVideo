#include "MediaPlayerEngine.h"
#include <QFileInfo>
#include <QDir>
#include <algorithm>
#include <random>

namespace core {

MediaPlayerEngine::MediaPlayerEngine(QObject *parent)
    : QObject(parent),
      m_player(new QMediaPlayer(this)),
      m_audioOutput(new QAudioOutput(this)) {
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(0.7f);
    m_lastVolume = 70;

    connect(m_player, &QMediaPlayer::positionChanged, this, &MediaPlayerEngine::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &MediaPlayerEngine::onDurationChanged);
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &MediaPlayerEngine::playbackStateChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &MediaPlayerEngine::onMediaStatusChanged);
    connect(m_player, &QMediaPlayer::errorOccurred, this, &MediaPlayerEngine::onPlayerError);
}

void MediaPlayerEngine::setVideoOutput(QObject *videoOutput) {
    m_player->setVideoOutput(videoOutput);
}

void MediaPlayerEngine::play() {
    if (m_currentIndex < 0 && !m_playlist.isEmpty()) {
        playIndex(0);
        return;
    }
    m_player->play();
}

void MediaPlayerEngine::pause() {
    m_player->pause();
}

void MediaPlayerEngine::togglePlay() {
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        pause();
    } else {
        play();
    }
}

void MediaPlayerEngine::stop() {
    m_player->stop();
}

void MediaPlayerEngine::next() {
    if (m_playlist.isEmpty()) return;

    int nextIdx = getNextIndex();
    if (nextIdx >= 0) {
        playIndex(nextIdx);
    } else if (m_loopMode == LoopMode::All) {
        playIndex(0);
    } else {
        stop();
        seek(0);
    }
}

void MediaPlayerEngine::previous() {
    if (m_playlist.isEmpty()) return;

    // If more than 3 seconds in, restart current track
    if (m_player->position() > 3000) {
        seek(0);
        return;
    }

    int prevIdx = getPreviousIndex();
    if (prevIdx >= 0) {
        playIndex(prevIdx);
    } else if (m_loopMode == LoopMode::All) {
        playIndex(m_playlist.size() - 1);
    } else {
        seek(0);
    }
}

void MediaPlayerEngine::seek(qint64 positionMs) {
    qint64 clamped = std::clamp(positionMs, 0LL, m_player->duration());
    m_player->setPosition(clamped);
}

void MediaPlayerEngine::seekRelative(qint64 deltaMs) {
    seek(m_player->position() + deltaMs);
}

void MediaPlayerEngine::setVolume(int volumePercent) {
    int clamped = std::clamp(volumePercent, 0, 100);
    m_lastVolume = clamped;
    m_audioOutput->setVolume(static_cast<float>(clamped) / 100.0f);
    if (clamped > 0 && m_audioOutput->isMuted()) {
        m_audioOutput->setMuted(false);
        emit mutedChanged(false);
    }
    emit volumeChanged(clamped);
}

int MediaPlayerEngine::volume() const {
    return qRound(m_audioOutput->volume() * 100.0f);
}

void MediaPlayerEngine::setMuted(bool muted) {
    m_audioOutput->setMuted(muted);
    emit mutedChanged(muted);
}

bool MediaPlayerEngine::isMuted() const {
    return m_audioOutput->isMuted();
}

void MediaPlayerEngine::toggleMute() {
    setMuted(!isMuted());
}

void MediaPlayerEngine::setPlaybackRate(qreal rate) {
    if (rate <= 0.0) return;
    m_playbackRate = rate;
    m_player->setPlaybackRate(rate);
    emit playbackRateChanged(rate);
}

qreal MediaPlayerEngine::playbackRate() const {
    return m_playbackRate;
}

void MediaPlayerEngine::setLoopMode(LoopMode mode) {
    if (m_loopMode != mode) {
        m_loopMode = mode;
        emit loopModeChanged(m_loopMode);
    }
}

LoopMode MediaPlayerEngine::loopMode() const {
    return m_loopMode;
}

void MediaPlayerEngine::cycleLoopMode() {
    switch (m_loopMode) {
    case LoopMode::None:
        setLoopMode(LoopMode::All);
        break;
    case LoopMode::All:
        setLoopMode(LoopMode::One);
        break;
    case LoopMode::One:
        setLoopMode(LoopMode::None);
        break;
    }
}

void MediaPlayerEngine::setShuffle(bool enabled) {
    if (m_isShuffle != enabled) {
        m_isShuffle = enabled;
        if (m_isShuffle) {
            rebuildShuffleList();
        }
        emit shuffleChanged(m_isShuffle);
    }
}

bool MediaPlayerEngine::isShuffle() const {
    return m_isShuffle;
}

void MediaPlayerEngine::toggleShuffle() {
    setShuffle(!m_isShuffle);
}

static bool isSameMedia(const QUrl &u1, const QUrl &u2) {
    if (u1 == u2) return true;
    if (u1.isLocalFile() && u2.isLocalFile()) {
        QString p1 = QDir::cleanPath(u1.toLocalFile()).toLower();
        QString p2 = QDir::cleanPath(u2.toLocalFile()).toLower();
        return p1 == p2;
    }
    return false;
}

bool MediaPlayerEngine::addMedia(const QUrl &url, bool autoPlayIfEmpty) {
    bool wasEmpty = m_playlist.isEmpty();

    for (int i = 0; i < m_playlist.size(); ++i) {
        if (isSameMedia(m_playlist[i].url, url)) {
            if (autoPlayIfEmpty && wasEmpty) {
                playIndex(i);
            }
            return false;
        }
    }

    PlaylistItem item;
    item.url = url;
    if (url.isLocalFile()) {
        item.title = QFileInfo(url.toLocalFile()).fileName();
    } else {
        item.title = url.fileName();
        if (item.title.isEmpty()) item.title = url.toString();
    }
    m_playlist.append(item);

    if (m_isShuffle) {
        rebuildShuffleList();
    }

    emit playlistUpdated();

    if (wasEmpty && autoPlayIfEmpty) {
        playIndex(0);
    }
    return true;
}

int MediaPlayerEngine::addMediaList(const QList<QUrl> &urls, bool autoPlayIfEmpty) {
    if (urls.isEmpty()) return 0;

    bool wasEmpty = m_playlist.isEmpty();
    int addedCount = 0;
    int firstMatchedIndex = -1;

    for (const auto &url : urls) {
        bool duplicate = false;
        for (int i = 0; i < m_playlist.size(); ++i) {
            if (isSameMedia(m_playlist[i].url, url)) {
                duplicate = true;
                if (firstMatchedIndex < 0) {
                    firstMatchedIndex = i;
                }
                break;
            }
        }
        if (duplicate) continue;

        PlaylistItem item;
        item.url = url;
        if (url.isLocalFile()) {
            item.title = QFileInfo(url.toLocalFile()).fileName();
        } else {
            item.title = url.fileName();
            if (item.title.isEmpty()) item.title = url.toString();
        }
        m_playlist.append(item);
        addedCount++;
    }

    if (addedCount > 0) {
        if (m_isShuffle) {
            rebuildShuffleList();
        }
        emit playlistUpdated();
    }

    if (wasEmpty && autoPlayIfEmpty) {
        if (!m_playlist.isEmpty()) {
            playIndex(0);
        } else if (firstMatchedIndex >= 0) {
            playIndex(firstMatchedIndex);
        }
    }

    return addedCount;
}

void MediaPlayerEngine::playIndex(int index) {
    if (index < 0 || index >= m_playlist.size()) return;

    m_currentIndex = index;
    const auto &item = m_playlist.at(m_currentIndex);

    m_player->setSource(item.url);
    m_player->play();

    emit currentTrackChanged(m_currentIndex, item);
}

void MediaPlayerEngine::removeMedia(int index) {
    if (index < 0 || index >= m_playlist.size()) return;

    bool wasPlayingThis = (index == m_currentIndex);
    m_playlist.removeAt(index);

    if (m_isShuffle) {
        rebuildShuffleList();
    }

    if (m_playlist.isEmpty()) {
        m_currentIndex = -1;
        stop();
        m_player->setSource(QUrl());
    } else if (wasPlayingThis) {
        int nextToPlay = (index < m_playlist.size()) ? index : 0;
        playIndex(nextToPlay);
    } else if (index < m_currentIndex) {
        m_currentIndex--;
    }

    emit playlistUpdated();
}

void MediaPlayerEngine::clearPlaylist() {
    stop();
    m_player->setSource(QUrl());
    m_playlist.clear();
    m_shuffledOrder.clear();
    m_currentIndex = -1;
    emit playlistUpdated();
}

PlaylistItem MediaPlayerEngine::currentItem() const {
    if (m_currentIndex >= 0 && m_currentIndex < m_playlist.size()) {
        return m_playlist.at(m_currentIndex);
    }
    return PlaylistItem{};
}

qint64 MediaPlayerEngine::position() const {
    return m_player->position();
}

qint64 MediaPlayerEngine::duration() const {
    return m_player->duration();
}

QMediaPlayer::PlaybackState MediaPlayerEngine::playbackState() const {
    return m_player->playbackState();
}

bool MediaPlayerEngine::hasNext() const {
    if (m_playlist.isEmpty()) return false;
    if (m_loopMode == LoopMode::All) return true;
    return getNextIndex() >= 0;
}

bool MediaPlayerEngine::hasPrevious() const {
    if (m_playlist.isEmpty()) return false;
    if (m_loopMode == LoopMode::All) return true;
    return getPreviousIndex() >= 0;
}

void MediaPlayerEngine::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        if (m_loopMode == LoopMode::One) {
            seek(0);
            play();
        } else if (hasNext()) {
            next();
        } else {
            seek(0);
            pause();
        }
    }
}

void MediaPlayerEngine::onDurationChanged(qint64 durationMs) {
    if (m_currentIndex >= 0 && m_currentIndex < m_playlist.size()) {
        m_playlist[m_currentIndex].durationMs = durationMs;
        emit playlistUpdated();
    }
    emit durationChanged(durationMs);
}

void MediaPlayerEngine::onPlayerError(QMediaPlayer::Error error, const QString &errorString) {
    if (error != QMediaPlayer::NoError) {
        emit mediaError(errorString);
    }
}

void MediaPlayerEngine::rebuildShuffleList() {
    m_shuffledOrder.clear();
    for (int i = 0; i < m_playlist.size(); ++i) {
        m_shuffledOrder.append(i);
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_shuffledOrder.begin(), m_shuffledOrder.end(), g);
}

int MediaPlayerEngine::getNextIndex() const {
    if (m_playlist.isEmpty()) return -1;

    if (m_isShuffle && !m_shuffledOrder.isEmpty()) {
        int currentPosInShuffle = m_shuffledOrder.indexOf(m_currentIndex);
        if (currentPosInShuffle >= 0 && currentPosInShuffle + 1 < m_shuffledOrder.size()) {
            return m_shuffledOrder.at(currentPosInShuffle + 1);
        }
        if (m_loopMode == LoopMode::All) {
            return m_shuffledOrder.first();
        }
        return -1;
    }

    if (m_currentIndex + 1 < m_playlist.size()) {
        return m_currentIndex + 1;
    }
    if (m_loopMode == LoopMode::All) {
        return 0;
    }
    return -1;
}

int MediaPlayerEngine::getPreviousIndex() const {
    if (m_playlist.isEmpty()) return -1;

    if (m_isShuffle && !m_shuffledOrder.isEmpty()) {
        int currentPosInShuffle = m_shuffledOrder.indexOf(m_currentIndex);
        if (currentPosInShuffle > 0) {
            return m_shuffledOrder.at(currentPosInShuffle - 1);
        }
        if (m_loopMode == LoopMode::All) {
            return m_shuffledOrder.last();
        }
        return -1;
    }

    if (m_currentIndex - 1 >= 0) {
        return m_currentIndex - 1;
    }
    if (m_loopMode == LoopMode::All) {
        return m_playlist.size() - 1;
    }
    return -1;
}

} // namespace core
