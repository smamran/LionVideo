#include <QApplication>
#include "src/ui/MainWindow.h"
#include "src/utils/IconHelper.h"

int main(int argc, char *argv[]) {
    // Retain and configure FFmpeg multimedia backend for optimal stability
    qputenv("QT_MEDIA_BACKEND", "ffmpeg");
    qputenv("FFMPEG_OPT_hwaccel", "none");
    qputenv("FFMPEG_LOG_LEVEL", "quiet");

    QApplication app(argc, argv);
    app.setApplicationName("Lion Media Player");
    app.setApplicationDisplayName("Lion Media Player");
    app.setOrganizationName("LionStudio");
    app.setApplicationVersion("2.0.0");

    // Set application and taskbar icon using provided branding
    app.setWindowIcon(utils::IconHelper::getAppIcon());

    ui::MainWindow window;
    window.show();

    // If file arguments are passed via command line, open them
    if (argc > 1) {
        QList<QUrl> urls;
        for (int i = 1; i < argc; ++i) {
            QString arg = QString::fromLocal8Bit(argv[i]);
            urls.append(QUrl::fromUserInput(arg));
        }
        if (!urls.isEmpty()) {
            QMetaObject::invokeMethod(&window, "onFilesDropped", Qt::QueuedConnection,
                                     Q_ARG(QList<QUrl>, urls));
        }
    }

    return app.exec();
}