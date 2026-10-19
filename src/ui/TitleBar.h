#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

namespace ui {

class TitleBar : public QWidget {
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);
    ~TitleBar() override = default;

    void setTitle(const QString &title);
    void updateWindowState(bool isMaximized);

signals:
    void minimizeClicked();
    void maximizeClicked();
    void closeClicked();
    void pipClicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    QLabel *m_iconLabel = nullptr;
    QLabel *m_titleLabel = nullptr;
    QPushButton *m_pipBtn = nullptr;
    QPushButton *m_minBtn = nullptr;
    QPushButton *m_maxBtn = nullptr;
    QPushButton *m_closeBtn = nullptr;

    bool m_isDragging = false;
    QPoint m_dragPosition;
};

} // namespace ui
