#pragma once

#include <QString>

namespace utils {

class StyleManager {
public:
    static QString getApplicationStyleSheet();
    static QString getControlBarStyleSheet();
    static QString getTitleBarStyleSheet();
    static QString getPlaylistDrawerStyleSheet();
    static QString getOSDStyleSheet();
    static QString getPlayButtonStyleSheet();
    static QString getStandardButtonStyleSheet();
    static QString getActiveButtonStyleSheet();
    static QString getVolumeSliderStyleSheet();
    static QString getMenuDarkStyleSheet();

    // Accent Colors
    static inline const QString AccentColor = "#60CDFF";
    static inline const QString AccentHoverColor = "#78D5FF";
    static inline const QString AccentPressedColor = "#44B8F0";
    static inline const QString DarkMicaBackground = "rgba(28, 28, 30, 0.92)";
    static inline const QString DarkPanelBorder = "rgba(255, 255, 255, 0.10)";
};

} // namespace utils
