#include "VideoWidget.h"
#include "../utils/IconHelper.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QVBoxLayout>
#include <QLabel>

namespace ui {

VideoWidget::VideoWidget(QWidget *parent)
    : QGraphicsView(parent) {
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);

    m_videoItem = new QGraphicsVideoItem();
    m_scene->addItem(m_videoItem);

    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("background-color: #0C0C0E;");
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true);
    setAcceptDrops(true);

    // Setup placeholder
    m_placeholderWidget = new QWidget(this);
    m_placeholderWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    auto *placeholderLayout = new QVBoxLayout(m_placeholderWidget);
    placeholderLayout->setAlignment(Qt::AlignCenter);
    placeholderLayout->setSpacing(12);

    auto *iconLabel = new QLabel(m_placeholderWidget);
    iconLabel->setFixedSize(72, 72);
    iconLabel->setScaledContents(true);
    iconLabel->setPixmap(utils::IconHelper::getAppIcon().pixmap(72, 72));
    iconLabel->setAlignment(Qt::AlignCenter);

    auto *titleLabel = new QLabel("Lion Media Player", m_placeholderWidget);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: 600; color: #FFFFFF;");
    titleLabel->setAlignment(Qt::AlignCenter);

    auto *descLabel = new QLabel("Drag and drop audio or video files here, or press 'O' to Open", m_placeholderWidget);
    descLabel->setStyleSheet("font-size: 13px; color: #8E8E93;");
    descLabel->setAlignment(Qt::AlignCenter);

    placeholderLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
    placeholderLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    placeholderLayout->addWidget(descLabel, 0, Qt::AlignCenter);

    connect(m_videoItem, &QGraphicsVideoItem::nativeSizeChanged, this, [this]() {
        updateVideoBounds();
    });
}

void VideoWidget::setPiPMode(bool isPiP) {
    m_isPiP = isPiP;
    m_leftPressed = false;
    m_isDragging = false;
    unsetCursor();
}

void VideoWidget::updateVideoBounds() {
    if (m_videoItem && scene()) {
        fitInView(m_videoItem, Qt::KeepAspectRatio);
    }
}

void VideoWidget::showPlaceholder(bool show) {
    if (m_placeholderWidget) {
        m_placeholderWidget->setVisible(show);
    }
}

void VideoWidget::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    updateVideoBounds();
    if (m_placeholderWidget) {
        m_placeholderWidget->setGeometry(rect());
    }
}

void VideoWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_leftPressed = false;
        m_isDragging = false;
        unsetCursor();
        emit doubleClicked();
        event->accept();
        return;
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}

void VideoWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_leftPressed = true;
        m_isDragging = false;
        m_dragStartGlobalPos = event->globalPosition().toPoint();
        m_dragStartWindowPos = window()->pos();
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void VideoWidget::mouseMoveEvent(QMouseEvent *event) {
    emit mouseMoved();
    if (m_leftPressed && (event->buttons() & Qt::LeftButton)) {
        if (m_isPiP) {
            QPoint currentGlobal = event->globalPosition().toPoint();
            QPoint delta = currentGlobal - m_dragStartGlobalPos;
            if (!m_isDragging && delta.manhattanLength() >= 4) {
                m_isDragging = true;
                setCursor(Qt::ClosedHandCursor);
            }
            if (m_isDragging) {
                window()->move(m_dragStartWindowPos + delta);
                event->accept();
                return;
            }
        }
    }
    QGraphicsView::mouseMoveEvent(event);
}

void VideoWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_leftPressed) {
        m_leftPressed = false;
        if (m_isDragging) {
            m_isDragging = false;
            unsetCursor();
            event->accept();
            return;
        }
        unsetCursor();
        emit singleClicked();
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void VideoWidget::wheelEvent(QWheelEvent *event) {
    int delta = event->angleDelta().y();
    emit wheelScrolled(delta);
    event->accept();
}

void VideoWidget::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void VideoWidget::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void VideoWidget::dropEvent(QDropEvent *event) {
    const QList<QUrl> urls = event->mimeData()->urls();
    if (!urls.isEmpty()) {
        emit filesDropped(urls);
        event->acceptProposedAction();
    }
}

} // namespace ui
