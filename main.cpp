#include <QApplication>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QWheelEvent>
#include <QStyle>
#include <QTime>
#include <QShortcut>
#include <QKeySequence>

// ১. কাস্টম সিকবার
class ClickableSlider : public QSlider
{
public:
    explicit ClickableSlider(Qt::Orientation orientation, QWidget* parent = nullptr)
        : QSlider(orientation, parent)
    {
        setFocusPolicy(Qt::NoFocus);
    }

protected:
    void mousePressEvent(QMouseEvent* event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            int value;
            if (orientation() == Qt::Horizontal)
            {
                value = QStyle::sliderValueFromPosition(minimum(), maximum(), event->position().x(), width());
            }
            else
            {
                value = QStyle::sliderValueFromPosition(minimum(), maximum(), event->position().y(), height());
            }
            setValue(value);
            emit sliderMoved(value);
            event->accept();
        }
        QSlider::mousePressEvent(event);
    }

    void wheelEvent(QWheelEvent* event) override
    {
        event->ignore();
    }
};

// ২. ভিডিও রেন্ডারিং ভিউ
class ResizableVideoView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ResizableVideoView(QWidget* parent = nullptr) : QGraphicsView(parent)
    {
        scene = new QGraphicsScene(this);
        setScene(scene);

        videoItem = new QGraphicsVideoItem();
        scene->addItem(videoItem);

        setFrameShape(QFrame::NoFrame);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setStyleSheet("background: black;");
        setFocusPolicy(Qt::NoFocus);
    }

    QGraphicsVideoItem* getVideoItem() const { return videoItem; }

    void updateVideoBounds()
    {
        if (videoItem)
        {
            fitInView(videoItem, Qt::KeepAspectRatio);
        }
    }

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QGraphicsView::resizeEvent(event);
        updateVideoBounds();
    }

    void mouseDoubleClickEvent(QMouseEvent* event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            emit doubleClicked();
            event->accept();
        }
        else
        {
            QGraphicsView::mouseDoubleClickEvent(event);
        }
    }

    void wheelEvent(QWheelEvent* event) override
    {
        event->ignore();
    }

signals:
    void doubleClicked();

private:
    QGraphicsScene* scene;
    QGraphicsVideoItem* videoItem;
};

// ৩. মেইন প্লেয়ার উইন্ডো
class VideoPlayerWindow : public QWidget
{
    Q_OBJECT

public:
    VideoPlayerWindow(QWidget* parent = nullptr) : QWidget(parent)
    {
        setWindowTitle("Advanced Media Player");
        resize(900, 550);

        player = new QMediaPlayer(this);
        audioOutput = new QAudioOutput(this);
        videoView = new ResizableVideoView(this);

        player->setAudioOutput(audioOutput);
        player->setVideoOutput(videoView->getVideoItem());

        // UI কন্ট্রোলস
        openBtn = new QPushButton("Open", this);
        playBtn = new QPushButton("Play", this);
        loopBtn = new QPushButton("Loop: Off", this);
        muteBtn = new QPushButton("Mute", this);

        openBtn->setFocusPolicy(Qt::NoFocus);
        playBtn->setFocusPolicy(Qt::NoFocus);
        loopBtn->setFocusPolicy(Qt::NoFocus);
        muteBtn->setFocusPolicy(Qt::NoFocus);

        timeLabel = new QLabel("00:00 / 00:00", this);
        positionSlider = new ClickableSlider(Qt::Horizontal, this);

        volumeSlider = new QSlider(Qt::Horizontal, this);
        volumeSlider->setRange(0, 100);
        volumeSlider->setValue(70);
        volumeSlider->setFixedWidth(100);
        volumeSlider->setFocusPolicy(Qt::NoFocus);
        audioOutput->setVolume(0.7f);

        // লেআউট সেটআপ
        controlContainer = new QWidget(this);
        QHBoxLayout* controlLayout = new QHBoxLayout(controlContainer);
        controlLayout->setContentsMargins(10, 5, 10, 5);

        controlLayout->addWidget(openBtn);
        controlLayout->addWidget(playBtn);
        controlLayout->addWidget(loopBtn);
        controlLayout->addWidget(positionSlider, 1);
        controlLayout->addWidget(timeLabel);
        controlLayout->addWidget(muteBtn);
        controlLayout->addWidget(volumeSlider);

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);
        mainLayout->addWidget(videoView, 1);
        mainLayout->addWidget(controlContainer, 0);

        // সিগন্যাল কানেকশন
        connect(videoView, &ResizableVideoView::doubleClicked, this, &VideoPlayerWindow::toggleFullScreen);

        connect(openBtn, &QPushButton::clicked, this, &VideoPlayerWindow::openFile);
        connect(playBtn, &QPushButton::clicked, this, &VideoPlayerWindow::togglePlay);
        connect(loopBtn, &QPushButton::clicked, this, &VideoPlayerWindow::toggleLoop);
        connect(muteBtn, &QPushButton::clicked, this, &VideoPlayerWindow::toggleMute);

        connect(player, &QMediaPlayer::positionChanged, this, &VideoPlayerWindow::updatePosition);
        connect(player, &QMediaPlayer::durationChanged, this, &VideoPlayerWindow::updateDuration);
        connect(positionSlider, &QSlider::sliderMoved, player, &QMediaPlayer::setPosition);

        connect(volumeSlider, &QSlider::valueChanged, this, &VideoPlayerWindow::setVolume);

        // ভিডিও শেষ হওয়ার হ্যান্ডলিং
        connect(player, &QMediaPlayer::mediaStatusChanged, this, &VideoPlayerWindow::handleMediaStatus);

        // ভিডিও সাইজ ফিট করার সমাধান
        connect(videoView->getVideoItem(), &QGraphicsVideoItem::nativeSizeChanged, this, [this]()
        {
            videoView->updateVideoBounds();
        });

        setupShortcuts();
        qApp->installEventFilter(this);
    }

protected:
    bool eventFilter(QObject* watched, QEvent* event) override
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            switch (keyEvent->key())
            {
            case Qt::Key_Left:
                seekRelative(-5000);
                return true;
            case Qt::Key_Right:
                seekRelative(5000);
                return true;
            case Qt::Key_Up:
                adjustVolume(5);
                return true;
            case Qt::Key_Down:
                adjustVolume(-5);
                return true;
            case Qt::Key_Space:
                togglePlay();
                return true;
            case Qt::Key_Escape:
                if (isFullScreen()) toggleFullScreen();
                return true;
            }
        }
        return QWidget::eventFilter(watched, event);
    }

    void wheelEvent(QWheelEvent* event) override
    {
        int delta = event->angleDelta().y();
        if (delta > 0) adjustVolume(5);
        else if (delta < 0) adjustVolume(-5);
        event->accept();
    }

private:
    void setupShortcuts()
    {
        QShortcut* openShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_O), this);
        connect(openShortcut, &QShortcut::activated, this, &VideoPlayerWindow::openFile);

        QShortcut* fullScreenShortcut = new QShortcut(QKeySequence(Qt::Key_F), this);
        connect(fullScreenShortcut, &QShortcut::activated, this, &VideoPlayerWindow::toggleFullScreen);
    }

    void handleMediaStatus(QMediaPlayer::MediaStatus status)
    {
        if (status == QMediaPlayer::EndOfMedia)
        {
            if (isLooping)
            {
                player->setPosition(0);
                player->play();
            }
            else
            {
                player->setPosition(0);
                positionSlider->setValue(0);
                playBtn->setText("Play");
            }
        }
    }

    void toggleLoop()
    {
        isLooping = !isLooping;
        if (isLooping)
        {
            loopBtn->setText("Loop: On");
        }
        else
        {
            loopBtn->setText("Loop: Off");
        }
    }

    void seekRelative(qint64 msecs)
    {
        qint64 targetPos = player->position() + msecs;
        targetPos = qBound(0LL, targetPos, player->duration());
        player->setPosition(targetPos);
    }

    void adjustVolume(int delta)
    {
        int newVolume = qBound(0, volumeSlider->value() + delta, 100);
        volumeSlider->setValue(newVolume);
    }

    void toggleFullScreen()
    {
        if (isFullScreen())
        {
            showNormal();
            controlContainer->show();
        }
        else
        {
            controlContainer->hide();
            showFullScreen();
        }
        videoView->updateVideoBounds();
    }

    void openFile()
    {
        QString fileName = QFileDialog::getOpenFileName(this, "Open Media File", "",
                                                        "Media Files (*.mp4 *.mkv *.avi *.mp3 *.wav *.webm)");
        if (!fileName.isEmpty())
        {
            player->setSource(QUrl::fromLocalFile(fileName));
            player->play();
            playBtn->setText("Pause");
        }
    }

    void togglePlay()
    {
        if (player->playbackState() == QMediaPlayer::PlayingState)
        {
            player->pause();
            playBtn->setText("Play");
        }
        else
        {
            player->play();
            playBtn->setText("Pause");
        }
    }

    void toggleMute()
    {
        bool isMuted = audioOutput->isMuted();
        audioOutput->setMuted(!isMuted);
        muteBtn->setText(isMuted ? "Mute" : "Unmute");
    }

    void setVolume(int value)
    {
        float volume = value / 100.0f;
        audioOutput->setVolume(volume);
        if (volume > 0 && audioOutput->isMuted())
        {
            audioOutput->setMuted(false);
            muteBtn->setText("Mute");
        }
    }

    void updatePosition(qint64 position)
    {
        if (!positionSlider->isSliderDown())
        {
            positionSlider->setValue(position);
        }
        updateTimeLabel(position, player->duration());
    }

    void updateDuration(qint64 duration)
    {
        positionSlider->setRange(0, duration);
        updateTimeLabel(player->position(), duration);
    }

    void updateTimeLabel(qint64 position, qint64 duration)
    {
        QTime posTime(0, 0, 0);
        posTime = posTime.addMSecs(position);

        QTime durTime(0, 0, 0);
        durTime = durTime.addMSecs(duration);

        QString format = (duration >= 3600000) ? "hh:mm:ss" : "mm:ss";
        timeLabel->setText(posTime.toString(format) + " / " + durTime.toString(format));
    }

    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    ResizableVideoView* videoView;

    QPushButton* openBtn;
    QPushButton* playBtn;
    QPushButton* loopBtn;
    QPushButton* muteBtn;

    QSlider* positionSlider;
    QSlider* volumeSlider;
    QLabel* timeLabel;

    QWidget* controlContainer;
    bool isLooping = false;
};

#include "main.moc"

int main(int argc, char* argv[])
{
    // AV1 Hardware Decoding / Vulkan Error ফিক্স
    qputenv("QT_MEDIA_BACKEND", "ffmpeg");
    qputenv("FFMPEG_OPT_hwaccel", "none");
    qputenv("FFMPEG_LOG_LEVEL", "quiet");

    QApplication app(argc, argv);

    VideoPlayerWindow window;
    window.show();

    return app.exec();
}
