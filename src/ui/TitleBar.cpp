#include "TitleBar.h"
#include "../utils/IconHelper.h"
#include <QMouseEvent>
#include <QApplication>

namespace ui {

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent) {
    setObjectName("TitleBarWidget");
    setFixedHeight(36);
    setStyleSheet(
        "#TitleBarWidget {"
        "  background-color: #1A1A1E;"
        "  border-bottom: 1px solid rgba(255, 255, 255, 0.06);"
        "}"
        "#TitleLabel {"
        "  font-size: 12px;"
        "  font-weight: 500;"
        "  color: #D0D0D4;"
        "}"
        ".TitleBtn {"
        "  background: transparent;"
        "  border: none;"
        "  border-radius: 4px;"
        "  padding: 4px;"
        "}"
        ".TitleBtn:hover {"
        "  background-color: rgba(255, 255, 255, 0.08);"
        "}"
        ".TitleBtn:pressed {"
        "  background-color: rgba(255, 255, 255, 0.14);"
        "}"
        "#CloseBtn {"
        "  background: transparent;"
        "  border: none;"
        "  border-radius: 4px;"
        "  padding: 4px;"
        "}"
        "#CloseBtn:hover {"
        "  background-color: #E81123;"
        "}"
        "#CloseBtn:pressed {"
        "  background-color: #C40E1E;"
        "}"
    );

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 0, 8, 0);
    layout->setSpacing(6);

    // App Branding Icon
    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(18, 18);
    m_iconLabel->setScaledContents(true);
    QIcon appIcon = utils::IconHelper::getAppIcon();
    m_iconLabel->setPixmap(appIcon.pixmap(18, 18));
    layout->addWidget(m_iconLabel);

    // Title text
    m_titleLabel = new QLabel("Lion Media Player", this);
    m_titleLabel->setObjectName("TitleLabel");
    layout->addWidget(m_titleLabel);

    layout->addStretch();

    // Buttons
    m_pipBtn = new QPushButton(this);
    m_pipBtn->setProperty("class", "TitleBtn");
    m_pipBtn->setIcon(utils::IconHelper::pipIcon());
    m_pipBtn->setIconSize(QSize(14, 14));
    m_pipBtn->setFixedSize(30, 26);
    m_pipBtn->setToolTip("Picture-in-Picture (P)");
    m_pipBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_pipBtn, &QPushButton::clicked, this, &TitleBar::pipClicked);
    layout->addWidget(m_pipBtn);

    m_minBtn = new QPushButton(this);
    m_minBtn->setProperty("class", "TitleBtn");
    m_minBtn->setIcon(utils::IconHelper::windowMinIcon());
    m_minBtn->setIconSize(QSize(12, 12));
    m_minBtn->setFixedSize(30, 26);
    m_minBtn->setToolTip("Minimize");
    m_minBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_minBtn, &QPushButton::clicked, this, &TitleBar::minimizeClicked);
    layout->addWidget(m_minBtn);

    m_maxBtn = new QPushButton(this);
    m_maxBtn->setProperty("class", "TitleBtn");
    m_maxBtn->setIcon(utils::IconHelper::windowMaxIcon());
    m_maxBtn->setIconSize(QSize(12, 12));
    m_maxBtn->setFixedSize(30, 26);
    m_maxBtn->setToolTip("Maximize");
    m_maxBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_maxBtn, &QPushButton::clicked, this, &TitleBar::maximizeClicked);
    layout->addWidget(m_maxBtn);

    m_closeBtn = new QPushButton(this);
    m_closeBtn->setObjectName("CloseBtn");
    m_closeBtn->setIcon(utils::IconHelper::windowCloseIcon());
    m_closeBtn->setIconSize(QSize(12, 12));
    m_closeBtn->setFixedSize(32, 26);
    m_closeBtn->setToolTip("Close");
    m_closeBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_closeBtn, &QPushButton::clicked, this, &TitleBar::closeClicked);
    layout->addWidget(m_closeBtn);
}

void TitleBar::setTitle(const QString &title) {
    if (title.isEmpty()) {
        m_titleLabel->setText("Lion Media Player");
    } else {
        m_titleLabel->setText(QString("Lion Media Player - %1").arg(title));
    }
}

void TitleBar::updateWindowState(bool isMaximized) {
    if (isMaximized) {
        m_maxBtn->setIcon(utils::IconHelper::windowRestoreIcon());
        m_maxBtn->setToolTip("Restore");
    } else {
        m_maxBtn->setIcon(utils::IconHelper::windowMaxIcon());
        m_maxBtn->setToolTip("Maximize");
    }
}

void TitleBar::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragPosition = event->globalPosition().toPoint() - window()->frameGeometry().topLeft();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        if (window()->isMaximized()) {
            // Restore from maximized while dragging
            double ratio = static_cast<double>(event->position().toPoint().x()) / static_cast<double>(width());
            window()->showNormal();
            int newX = static_cast<int>(ratio * window()->width());
            m_dragPosition = QPoint(newX, event->position().toPoint().y());
        }
        window()->move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit maximizeClicked();
        event->accept();
        return;
    }
    QWidget::mouseDoubleClickEvent(event);
}

} // namespace ui
