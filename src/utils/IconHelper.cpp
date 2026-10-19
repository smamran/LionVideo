#include "IconHelper.h"
#include <QPainter>
#include <QSvgRenderer>

namespace utils {

QIcon IconHelper::getAppIcon() {
    // 1. Attempt to load provided player.webp via resource system
    QIcon icon(":/player.webp");
    if (!icon.isNull() && !icon.pixmap(32, 32).isNull()) {
        return icon;
    }

    // 2. Fallback to PNG if WebP image plugin is unavailable in current Qt build
    QIcon pngIcon(":/player.png");
    if (!pngIcon.isNull() && !pngIcon.pixmap(32, 32).isNull()) {
        return pngIcon;
    }

    // 3. Fallback to ICO
    QIcon icoIcon(":/player.ico");
    if (!icoIcon.isNull()) {
        return icoIcon;
    }

    return QIcon();
}

QIcon IconHelper::getIcon(const QString &resourcePath, const QColor &color) {
    if (color == Qt::white) {
        return QIcon(resourcePath);
    }
    return QIcon(getColoredPixmap(resourcePath, color, QSize(24, 24)));
}

QPixmap IconHelper::getColoredPixmap(const QString &resourcePath, const QColor &color, const QSize &size) {
    QSvgRenderer renderer(resourcePath);
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    renderer.render(&painter);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), color);
    painter.end();

    return pixmap;
}

QIcon IconHelper::playIcon() {
    return QIcon(":/resources/icons/play.svg");
}

QIcon IconHelper::pauseIcon() {
    return QIcon(":/resources/icons/pause.svg");
}

QIcon IconHelper::prevIcon() {
    return QIcon(":/resources/icons/prev.svg");
}

QIcon IconHelper::nextIcon() {
    return QIcon(":/resources/icons/next.svg");
}

QIcon IconHelper::volumeHighIcon() {
    return QIcon(":/resources/icons/volume_high.svg");
}

QIcon IconHelper::volumeMedIcon() {
    return QIcon(":/resources/icons/volume_med.svg");
}

QIcon IconHelper::volumeLowIcon() {
    return QIcon(":/resources/icons/volume_low.svg");
}

QIcon IconHelper::volumeMutedIcon() {
    return QIcon(":/resources/icons/volume_muted.svg");
}

QIcon IconHelper::fullscreenIcon() {
    return QIcon(":/resources/icons/fullscreen.svg");
}

QIcon IconHelper::fullscreenExitIcon() {
    return QIcon(":/resources/icons/fullscreen_exit.svg");
}

QIcon IconHelper::loopIcon() {
    return QIcon(":/resources/icons/loop.svg");
}

QIcon IconHelper::loopOneIcon() {
    return QIcon(":/resources/icons/loop_one.svg");
}

QIcon IconHelper::shuffleIcon() {
    return QIcon(":/resources/icons/shuffle.svg");
}

QIcon IconHelper::pipIcon() {
    return QIcon(":/resources/icons/pip.svg");
}

QIcon IconHelper::playlistIcon() {
    return QIcon(":/resources/icons/playlist.svg");
}

QIcon IconHelper::speedIcon() {
    return QIcon(":/resources/icons/speed.svg");
}

QIcon IconHelper::folderOpenIcon() {
    return QIcon(":/resources/icons/folder_open.svg");
}

QIcon IconHelper::windowMinIcon() {
    return QIcon(":/resources/icons/window_min.svg");
}

QIcon IconHelper::windowMaxIcon() {
    return QIcon(":/resources/icons/window_max.svg");
}

QIcon IconHelper::windowRestoreIcon() {
    return QIcon(":/resources/icons/window_restore.svg");
}

QIcon IconHelper::windowCloseIcon() {
    return QIcon(":/resources/icons/window_close.svg");
}

QIcon IconHelper::trashIcon() {
    return QIcon(":/resources/icons/trash.svg");
}

QIcon IconHelper::plusIcon() {
    return QIcon(":/resources/icons/plus.svg");
}

} // namespace utils
