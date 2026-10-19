#include "OSDLabel.h"
#include <algorithm>

namespace ui {

OSDLabel::OSDLabel(QWidget *parent)
    : QWidget(parent) {
    setObjectName("OSDContainer");
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setStyleSheet(
        "#OSDContainer {"
        "  background-color: rgba(30, 30, 34, 0.92);"
        "  border: 1px solid rgba(255, 255, 255, 0.16);"
        "  border-radius: 8px;"
        "}"
        "QLabel {"
        "  color: #FFFFFF;"
        "  font-size: 13px;"
        "  font-weight: 600;"
        "  background: transparent;"
        "}"
    );

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(14, 8, 14, 8);
    layout->setSpacing(8);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(18, 18);
    m_iconLabel->setScaledContents(true);
    m_iconLabel->hide();

    m_textLabel = new QLabel(this);
    m_textLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_iconLabel);
    layout->addWidget(m_textLabel);

    m_opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(m_opacityEffect);
    m_opacityEffect->setOpacity(0.0);

    m_fadeAnimation = new QPropertyAnimation(m_opacityEffect, "opacity", this);

    m_hideTimer = new QTimer(this);
    m_hideTimer->setSingleShot(true);
    connect(m_hideTimer, &QTimer::timeout, this, &OSDLabel::startFadeOut);

    hide();
}

void OSDLabel::showNotification(const QString &text, const QIcon &icon, int durationMs) {
    m_hideTimer->stop();
    m_fadeAnimation->stop();

    if (!icon.isNull()) {
        m_iconLabel->setPixmap(icon.pixmap(18, 18));
        m_iconLabel->show();
    } else {
        m_iconLabel->hide();
    }

    m_textLabel->setText(text);
    adjustSize();

    if (parentWidget()) {
        reposition(parentWidget()->size());
    }

    show();
    raise();

    // Smooth fade in
    m_fadeAnimation->setDuration(120);
    m_fadeAnimation->setStartValue(m_opacityEffect->opacity());
    m_fadeAnimation->setEndValue(1.0);
    m_fadeAnimation->start();

    m_hideTimer->start(durationMs);
}

void OSDLabel::startFadeOut() {
    m_fadeAnimation->stop();
    m_fadeAnimation->setDuration(250);
    m_fadeAnimation->setStartValue(m_opacityEffect->opacity());
    m_fadeAnimation->setEndValue(0.0);
    connect(m_fadeAnimation, &QPropertyAnimation::finished, this, [this]() {
        if (m_opacityEffect->opacity() <= 0.01) {
            hide();
        }
    });
    m_fadeAnimation->start();
}

void OSDLabel::reposition(const QSize &parentSize) {
    int x = parentSize.width() - width() - 24;
    int y = 24;
    move(std::max(16, x), y);
}

} // namespace ui
