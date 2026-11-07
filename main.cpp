#include <QApplication>
#include <QPalette>
#include <QColor>
#include <QStyleHints>
#include "src/ui/MainWindow.h"
#include "src/utils/IconHelper.h"

int main(int argc, char *argv[]) {
    // Multimedia backend configuration
#if defined(Q_OS_WIN)
    // On Windows, explicitly select FFmpeg backend for optimal stability
    qputenv("QT_MEDIA_BACKEND", "ffmpeg");
#else
    // On Linux/Ubuntu, let Qt select the available backend (e.g. GStreamer on Qt 6.2-6.4 or FFmpeg on Qt 6.5+)
    // while honoring any user-specified QT_MEDIA_BACKEND override

    // Prefer X11/XWayland (xcb) by default on Linux when an X display is available,
    // ensuring Picture-in-Picture "Always on Top" (Qt::WindowStaysOnTopHint / _NET_WM_STATE_ABOVE)
    // is fully honored by GNOME Mutter and X11 compositors.
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        if (!qEnvironmentVariableIsEmpty("DISPLAY")) {
            qputenv("QT_QPA_PLATFORM", "xcb");
        }
    }
#endif
    qputenv("FFMPEG_OPT_hwaccel", "none");
    qputenv("FFMPEG_LOG_LEVEL", "quiet");

    QApplication app(argc, argv);
    app.setApplicationName("Lion Media Player");
    app.setApplicationDisplayName("Lion Media Player");
    app.setOrganizationName("LionStudio");
    app.setApplicationVersion("2.0.0");
    app.setDesktopFileName("lionvideo");

    // Configure consistent dark theme palette across Qt versions and platforms,
    // ensuring Ubuntu 22.04 LTS (Qt 6.2) and Windows match Ubuntu 24.04/26.04 (Qt 6.5+)
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(26, 26, 30));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(18, 18, 20));
    darkPalette.setColor(QPalette::AlternateBase, QColor(26, 26, 30));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(36, 36, 38));
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(26, 26, 30));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(96, 205, 255));
    darkPalette.setColor(QPalette::Highlight, QColor(96, 205, 255));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(darkPalette);

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    if (app.styleHints()) {
        app.styleHints()->setColorScheme(Qt::ColorScheme::Dark);
    }
#endif

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