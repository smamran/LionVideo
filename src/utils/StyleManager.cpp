#include "StyleManager.h"

namespace utils {

QString StyleManager::getApplicationStyleSheet() {
    return QString(R"(
        QWidget {
            font-family: 'Segoe UI Variable Display', 'Segoe UI', -apple-system, sans-serif;
            font-size: 13px;
            color: #FFFFFF;
        }

        QToolTip {
            background-color: rgba(36, 36, 38, 0.95);
            color: #FFFFFF;
            border: 1px solid rgba(255, 255, 255, 0.15);
            border-radius: 6px;
            padding: 6px 10px;
            font-size: 12px;
        }

        /* Custom Scrollbar */
        QScrollBar:vertical {
            background: transparent;
            width: 8px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background: rgba(255, 255, 255, 0.2);
            min-height: 25px;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical:hover {
            background: rgba(255, 255, 255, 0.35);
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: transparent;
        }

        /* Menu */
        QMenu {
            background-color: rgba(36, 36, 38, 0.96);
            border: 1px solid rgba(255, 255, 255, 0.12);
            border-radius: 8px;
            padding: 6px;
        }
        QMenu::item {
            padding: 6px 24px 6px 14px;
            border-radius: 4px;
            color: #FFFFFF;
        }
        QMenu::item:selected {
            background-color: rgba(255, 255, 255, 0.1);
            color: #60CDFF;
        }
        QMenu::separator {
            height: 1px;
            background: rgba(255, 255, 255, 0.1);
            margin: 4px 6px;
        }
    )");
}

QString StyleManager::getControlBarStyleSheet() {
    return QString(R"(
        #ControlBarWidget {
            background-color: rgba(28, 28, 30, 0.90);
            border: 1px solid rgba(255, 255, 255, 0.12);
            border-radius: 12px;
        }
    )");
}

QString StyleManager::getTitleBarStyleSheet() {
    return QString(R"(
        #TitleBarWidget {
            background-color: #1A1A1C;
            border-bottom: 1px solid rgba(255, 255, 255, 0.05);
        }
        #TitleLabel {
            font-size: 12px;
            font-weight: 500;
            color: #CCCCCC;
        }
        #TitleBarBtn {
            background: transparent;
            border: none;
            border-radius: 4px;
            padding: 4px;
        }
        #TitleBarBtn:hover {
            background-color: rgba(255, 255, 255, 0.08);
        }
        #TitleBarBtn:pressed {
            background-color: rgba(255, 255, 255, 0.14);
        }
        #CloseBtn {
            background: transparent;
            border: none;
            border-radius: 4px;
            padding: 4px;
        }
        #CloseBtn:hover {
            background-color: #E81123;
        }
        #CloseBtn:pressed {
            background-color: #C40E1E;
        }
    )");
}

QString StyleManager::getPlaylistDrawerStyleSheet() {
    return QString(R"(
        #PlaylistDrawerWidget {
            background-color: rgba(24, 24, 26, 0.95);
            border-left: 1px solid rgba(255, 255, 255, 0.10);
        }
        #PlaylistTitleLabel {
            font-size: 14px;
            font-weight: 600;
            color: #FFFFFF;
        }
        #PlaylistCountLabel {
            font-size: 12px;
            color: #8E8E93;
        }
        QListWidget {
            background: transparent;
            border: none;
            outline: none;
            padding: 4px;
        }
        QListWidget::item {
            background: transparent;
            border-radius: 6px;
            padding: 8px 10px;
            margin-bottom: 2px;
            color: #E0E0E0;
        }
        QListWidget::item:hover {
            background-color: rgba(255, 255, 255, 0.06);
        }
        QListWidget::item:selected {
            background-color: rgba(96, 205, 255, 0.15);
            border-left: 3px solid #60CDFF;
            color: #60CDFF;
            font-weight: 600;
        }
    )");
}

QString StyleManager::getOSDStyleSheet() {
    return QString(R"(
        #OSDContainer {
            background-color: rgba(30, 30, 34, 0.92);
            border: 1px solid rgba(255, 255, 255, 0.14);
            border-radius: 8px;
            padding: 8px 16px;
        }
        #OSDText {
            color: #FFFFFF;
            font-size: 14px;
            font-weight: 600;
        }
    )");
}

QString StyleManager::getPlayButtonStyleSheet() {
    return QString(R"(
        QPushButton {
            background-color: #60CDFF;
            border: none;
            border-radius: 20px;
            padding: 8px;
        }
        QPushButton:hover {
            background-color: #78D5FF;
        }
        QPushButton:pressed {
            background-color: #44B8F0;
        }
    )");
}

QString StyleManager::getStandardButtonStyleSheet() {
    return QString(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            border-radius: 6px;
            padding: 6px;
            color: #E0E0E0;
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 0.08);
            color: #FFFFFF;
        }
        QPushButton:pressed {
            background-color: rgba(255, 255, 255, 0.15);
        }
        QPushButton:checked {
            background-color: rgba(96, 205, 255, 0.20);
            color: #60CDFF;
        }
    )");
}

QString StyleManager::getActiveButtonStyleSheet() {
    return QString(R"(
        QPushButton {
            background-color: rgba(96, 205, 255, 0.22);
            border: 1px solid rgba(96, 205, 255, 0.4);
            border-radius: 6px;
            padding: 6px;
            color: #60CDFF;
        }
        QPushButton:hover {
            background-color: rgba(96, 205, 255, 0.32);
        }
        QPushButton:pressed {
            background-color: rgba(96, 205, 255, 0.18);
        }
    )");
}

QString StyleManager::getVolumeSliderStyleSheet() {
    return QString(R"(
        QSlider {
            background: transparent;
        }
        QSlider::groove:horizontal {
            height: 4px;
            background: rgba(255, 255, 255, 0.20);
            border-radius: 2px;
        }
        QSlider::sub-page:horizontal {
            background: #60CDFF;
            border-radius: 2px;
        }
        QSlider::add-page:horizontal {
            background: rgba(255, 255, 255, 0.20);
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            background: #FFFFFF;
            width: 12px;
            height: 12px;
            margin: -4px 0;
            border-radius: 6px;
        }
        QSlider::handle:horizontal:hover {
            background: #78D5FF;
            width: 14px;
            height: 14px;
            margin: -5px 0;
            border-radius: 7px;
        }
    )");
}

QString StyleManager::getMenuDarkStyleSheet() {
    return QString(R"(
        QMenu {
            background-color: rgba(36, 36, 38, 0.96);
            border: 1px solid rgba(255, 255, 255, 0.14);
            border-radius: 8px;
            padding: 6px;
        }
        QMenu::item {
            padding: 6px 20px 6px 12px;
            border-radius: 4px;
            color: #FFFFFF;
        }
        QMenu::item:selected {
            background-color: rgba(255, 255, 255, 0.1);
            color: #60CDFF;
        }
        QMenu::item:checked {
            color: #60CDFF;
            font-weight: bold;
        }
        QMenu::separator {
            height: 1px;
            background: rgba(255, 255, 255, 0.12);
            margin: 4px 8px;
        }
    )");
}

} // namespace utils
