#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QLabel>
#include <QUrl>
#include <QList>

namespace ui {

class VideoWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget() override = default;

    [[nodiscard]] QGraphicsVideoItem* videoItem() const { return m_videoItem; }
    void updateVideoBounds();
    void showPlaceholder(bool show);
    void setPiPMode(bool isPiP);
    [[nodiscard]] bool isPiPMode() const { return m_isPiP; }

signals:
    void doubleClicked();
    void mouseMoved();
    void singleClicked();
    void wheelScrolled(int delta);
    void filesDropped(const QList<QUrl> &urls);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QGraphicsScene *m_scene = nullptr;
    QGraphicsVideoItem *m_videoItem = nullptr;
    QWidget *m_placeholderWidget = nullptr;

    bool m_isPiP = false;
    bool m_leftPressed = false;
    bool m_isDragging = false;
    QPoint m_dragStartGlobalPos;
    QPoint m_dragStartWindowPos;
};

} // namespace ui
