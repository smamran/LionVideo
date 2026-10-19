#include "VideoWidget.h"
#include "../utils/IconHelper.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QContextMenuEvent>

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
    viewport()->setMouseTracking(true);
    setAcceptDrops(true);

    // Setup placeholder
    m_placeholderWidget = new QWidget(viewport());
    m_placeholderWidget->installEventFilter(this);
    auto *placeholderLayout = new QVBoxLayout(m_placeholderWidget);
    placeholderLayout->setAlignment(Qt::AlignCenter);
    placeholderLayout->setSpacing(14);

    auto *iconLabel = new QLabel(m_placeholderWidget);
    iconLabel->setFixedSize(72, 72);
    iconLabel->setScaledContents(true);
    iconLabel->setPixmap(utils::IconHelper::getAppIcon().pixmap(72, 72));
    iconLabel->setAlignment(Qt::AlignCenter);

    auto *titleLabel = new QLabel("Lion Media Player", m_placeholderWidget);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: 600; color: #FFFFFF;");
    titleLabel->setAlignment(Qt::AlignCenter);

    auto *descLabel = new QLabel("Drag and drop audio or video files here, or browse", m_placeholderWidget);
    descLabel->setStyleSheet("font-size: 13px; color: #8E8E93;");
    descLabel->setAlignment(Qt::AlignCenter);

    auto *browseBtn = new QPushButton("  Browse Files", m_placeholderWidget);
    browseBtn->setIcon(utils::IconHelper::folderOpenIcon());
    browseBtn->setIconSize(QSize(16, 16));
    browseBtn->setCursor(Qt::PointingHandCursor);
    browseBtn->setFixedSize(150, 38);
    browseBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #60CDFF;"
        "  color: #000000;"
        "  font-size: 13px;"
        "  font-weight: 600;"
        "  border: none;"
        "  border-radius: 19px;"
        "  padding: 6px 16px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #78D5FF;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #44B8F0;"
        "}"
    );
    browseBtn->setToolTip("Browse Media Files (O)");
    connect(browseBtn, &QPushButton::clicked, this, &VideoWidget::openFileRequested);

    placeholderLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
    placeholderLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    placeholderLayout->addWidget(descLabel, 0, Qt::AlignCenter);
    placeholderLayout->addWidget(browseBtn, 0, Qt::AlignCenter);

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
        m_placeholderWidget->setGeometry(viewport()->rect());
    }
    emit widgetResized();
}

bool VideoWidget::viewportEvent(QEvent *event) {
    if (event->type() == QEvent::MouseMove) {
        emit mouseMoved();
        auto *mouseEv = static_cast<QMouseEvent*>(event);
        if (mouseEv && mouseEv->position().y() >= (viewport()->height() - 120)) {
            emit mouseNearBottom();
        }
    }
    return QGraphicsView::viewportEvent(event);
}

void VideoWidget::contextMenuEvent(QContextMenuEvent *event) {
    emit contextMenuRequested(event->globalPos());
    event->accept();
}

bool VideoWidget::eventFilter(QObject *watched, QEvent *event) {
    if (watched == m_placeholderWidget) {
        if (event->type() == QEvent::DragEnter || event->type() == QEvent::DragMove) {
            auto *dragEv = static_cast<QDropEvent*>(event);
            if (dragEv->mimeData()->hasUrls()) {
                dragEv->acceptProposedAction();
                return true;
            }
        } else if (event->type() == QEvent::Drop) {
            auto *dropEv = static_cast<QDropEvent*>(event);
            if (dropEv->mimeData()->hasUrls()) {
                emit filesDropped(dropEv->mimeData()->urls());
                dropEv->acceptProposedAction();
                return true;
            }
        }
    }
    return QGraphicsView::eventFilter(watched, event);
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
    } else if (event->button() == Qt::RightButton) {
        emit contextMenuRequested(event->globalPosition().toPoint());
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
