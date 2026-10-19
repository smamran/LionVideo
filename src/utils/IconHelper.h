#pragma once

#include <QIcon>
#include <QPixmap>
#include <QString>
#include <QColor>

namespace utils {

class IconHelper {
public:
    static QIcon getAppIcon();
    static QIcon getIcon(const QString &resourcePath, const QColor &color = Qt::white);
    static QPixmap getColoredPixmap(const QString &resourcePath, const QColor &color, const QSize &size);

    // Predefined UI icons
    static QIcon playIcon();
    static QIcon pauseIcon();
    static QIcon prevIcon();
    static QIcon nextIcon();
    static QIcon volumeHighIcon();
    static QIcon volumeMedIcon();
    static QIcon volumeLowIcon();
    static QIcon volumeMutedIcon();
    static QIcon fullscreenIcon();
    static QIcon fullscreenExitIcon();
    static QIcon loopIcon();
    static QIcon loopOneIcon();
    static QIcon shuffleIcon();
    static QIcon pipIcon();
    static QIcon playlistIcon();
    static QIcon speedIcon();
    static QIcon folderOpenIcon();
    static QIcon windowMinIcon();
    static QIcon windowMaxIcon();
    static QIcon windowRestoreIcon();
    static QIcon windowCloseIcon();
    static QIcon trashIcon();
    static QIcon plusIcon();
};

} // namespace utils
