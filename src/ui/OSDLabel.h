#pragma once

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>

namespace ui {

class OSDLabel : public QWidget {
    Q_OBJECT

public:
    explicit OSDLabel(QWidget *parent = nullptr);
    ~OSDLabel() override = default;

    void showNotification(const QString &text, const QIcon &icon = QIcon(), int durationMs = 1500);
    void reposition(const QSize &parentSize);

private slots:
    void startFadeOut();

private:
    QLabel *m_iconLabel = nullptr;
    QLabel *m_textLabel = nullptr;
    QGraphicsOpacityEffect *m_opacityEffect = nullptr;
    QPropertyAnimation *m_fadeAnimation = nullptr;
    QTimer *m_hideTimer = nullptr;
};

} // namespace ui
