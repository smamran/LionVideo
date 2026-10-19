#include "ClickableSlider.h"
#include "../utils/TimeFormatter.h"
#include <QPainter>
#include <QPainterPath>
#include <algorithm>

namespace ui {

ClickableSlider::ClickableSlider(Qt::Orientation orientation, QWidget *parent)
    : QSlider(orientation, parent) {
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setStyleSheet("background: transparent;");
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true);
    setFixedHeight(20);

    // Create preview bubble
    m_previewBubble = new QLabel(this);
    m_previewBubble->setStyleSheet(
        "QLabel {"
        "  background-color: rgba(30, 30, 34, 0.96);"
        "  color: #FFFFFF;"
        "  font-size: 11px;"
        "  font-weight: 600;"
        "  border: 1px solid rgba(255, 255, 255, 0.18);"
        "  border-radius: 4px;"
        "  padding: 3px 6px;"
        "}"
    );
    m_previewBubble->setAlignment(Qt::AlignCenter);
    m_previewBubble->hide();
}

int ClickableSlider::valueFromPosition(int mouseX) const {
    if (width() <= 0) return 0;
    double ratio = std::clamp(static_cast<double>(mouseX) / static_cast<double>(width()), 0.0, 1.0);
    return minimum() + static_cast<int>(ratio * (maximum() - minimum()));
}

void ClickableSlider::updatePreviewBubble(int mouseX) {
    if (maximum() <= minimum()) {
        hidePreviewBubble();
        return;
    }

    qint64 previewMs = valueFromPosition(mouseX);
    bool forceHours = maximum() >= 3600000;
    m_previewBubble->setText(utils::TimeFormatter::formatTime(previewMs, forceHours));
    m_previewBubble->adjustSize();

    int bubbleW = m_previewBubble->width();
    int bubbleH = m_previewBubble->height();
    int posX = std::clamp(mouseX - bubbleW / 2, 0, width() - bubbleW);
    int posY = -bubbleH - 4; // Float above slider

    m_previewBubble->move(posX, posY);
    m_previewBubble->show();
    m_previewBubble->raise();
}

void ClickableSlider::hidePreviewBubble() {
    if (m_previewBubble && !m_isDragging) {
        m_previewBubble->hide();
    }
}

void ClickableSlider::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        int val = valueFromPosition(event->position().toPoint().x());
        setValue(val);
        emit sliderMoved(val);
        emit seekRequested(val);
        updatePreviewBubble(event->position().toPoint().x());
        update();
        event->accept();
        return;
    }
    QSlider::mousePressEvent(event);
}

void ClickableSlider::mouseMoveEvent(QMouseEvent *event) {
    m_hoverX = event->position().toPoint().x();
    updatePreviewBubble(m_hoverX);

    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        int val = valueFromPosition(m_hoverX);
        setValue(val);
        emit sliderMoved(val);
        emit seekRequested(val);
    }
    update();
    QSlider::mouseMoveEvent(event);
}

void ClickableSlider::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        if (!rect().contains(event->position().toPoint())) {
            hidePreviewBubble();
        }
        update();
        event->accept();
        return;
    }
    QSlider::mouseReleaseEvent(event);
}

void ClickableSlider::enterEvent(QEnterEvent *event) {
    m_isHovered = true;
    m_hoverX = event->position().toPoint().x();
    updatePreviewBubble(m_hoverX);
    update();
    QSlider::enterEvent(event);
}

void ClickableSlider::leaveEvent(QEvent *event) {
    m_isHovered = false;
    m_hoverX = -1;
    hidePreviewBubble();
    update();
    QSlider::leaveEvent(event);
}

void ClickableSlider::wheelEvent(QWheelEvent *event) {
    event->ignore();
}

void ClickableSlider::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();
    int trackHeight = (m_isHovered || m_isDragging) ? 6 : 3;
    int trackY = (h - trackHeight) / 2;

    // Background track
    QPainterPath bgPath;
    bgPath.addRoundedRect(0, trackY, w, trackHeight, trackHeight / 2.0, trackHeight / 2.0);
    painter.fillPath(bgPath, QColor(255, 255, 255, 45));

    // Hover ghost track indicator
    if (m_isHovered && m_hoverX > 0 && maximum() > minimum()) {
        int hoverWidth = std::clamp(m_hoverX, 0, w);
        QPainterPath hoverPath;
        hoverPath.addRoundedRect(0, trackY, hoverWidth, trackHeight, trackHeight / 2.0, trackHeight / 2.0);
        painter.fillPath(hoverPath, QColor(255, 255, 255, 60));
    }

    // Active progress track
    double progressRatio = 0.0;
    if (maximum() > minimum()) {
        progressRatio = static_cast<double>(value() - minimum()) / static_cast<double>(maximum() - minimum());
    }
    progressRatio = std::clamp(progressRatio, 0.0, 1.0);
    int progressW = static_cast<int>(progressRatio * w);

    if (progressW > 0) {
        QPainterPath progressPath;
        progressPath.addRoundedRect(0, trackY, progressW, trackHeight, trackHeight / 2.0, trackHeight / 2.0);
        painter.fillPath(progressPath, QColor("#60CDFF")); // Fluent accent color
    }

    // Thumb handle (expand on hover)
    if (m_isHovered || m_isDragging) {
        int thumbRadius = 6;
        int thumbX = std::clamp(progressW, thumbRadius, w - thumbRadius);
        int thumbY = h / 2;

        // Subtle glow / border
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 80));
        painter.drawEllipse(QPoint(thumbX, thumbY), thumbRadius + 1, thumbRadius + 1);

        // Solid thumb
        painter.setBrush(QColor("#FFFFFF"));
        painter.drawEllipse(QPoint(thumbX, thumbY), thumbRadius, thumbRadius);
    }
}

} // namespace ui
