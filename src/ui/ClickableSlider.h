#pragma once

#include <QSlider>
#include <QLabel>
#include <QMouseEvent>
#include <QEnterEvent>

namespace ui {

class ClickableSlider : public QSlider {
    Q_OBJECT

public:
    explicit ClickableSlider(Qt::Orientation orientation, QWidget *parent = nullptr);
    ~ClickableSlider() override = default;

signals:
    void seekRequested(qint64 position);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    int valueFromPosition(int mouseX) const;
    void updatePreviewBubble(int mouseX);
    void hidePreviewBubble();

    QLabel *m_previewBubble = nullptr;
    bool m_isHovered = false;
    bool m_isDragging = false;
    int m_hoverX = -1;
};

} // namespace ui
