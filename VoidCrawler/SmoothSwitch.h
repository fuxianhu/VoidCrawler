#pragma once

#include <QWidget>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QPainter>

class SmoothSwitch : public QWidget
{
    Q_OBJECT
        Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY toggled)
        Q_PROPERTY(QColor trackColor READ trackColor WRITE setTrackColor)
        Q_PROPERTY(QColor thumbColor READ thumbColor WRITE setThumbColor)
        Q_PROPERTY(QColor activeTrackColor READ activeTrackColor WRITE setActiveTrackColor)
        Q_PROPERTY(qreal thumbPosition READ thumbPosition WRITE setThumbPosition)

public:
    explicit SmoothSwitch(QWidget* parent = nullptr);

    // 获取和设置开关状态
    bool isChecked() const;
    void setChecked(bool checked);

    // 颜色获取和设置
    QColor trackColor() const;
    void setTrackColor(const QColor& color);

    QColor thumbColor() const;
    void setThumbColor(const QColor& color);

    QColor activeTrackColor() const;
    void setActiveTrackColor(const QColor& color);

    // 动画持续时间设置
    void setAnimationDuration(int duration);
    int animationDuration() const;

    // 开关大小设置
    void setSwitchSize(int width, int height);
    QSize switchSize() const;

    // 获取当前位置（用于动画）
    qreal thumbPosition() const;
    void setThumbPosition(qreal position);

signals:
    void toggled(bool checked);  // 状态改变时发射的信号
    void clicked();              // 点击时发射的信号

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void setupAnimations();
    void updateSwitchGeometry();

    // 状态变量
    bool m_checked = false;
    bool m_hovered = false;
    bool m_pressed = false;

    // 颜色
    QColor m_trackColor = QColor(200, 200, 200);
    QColor m_thumbColor = QColor(255, 255, 255);
    QColor m_activeTrackColor = QColor(100, 150, 255);
    QColor m_hoverColor = QColor(230, 230, 230);
    QColor m_activeHoverColor = QColor(80, 130, 235);

    // 动画相关
    QPropertyAnimation* m_thumbAnimation;
    QPropertyAnimation* m_trackColorAnimation;
    qreal m_thumbPosition = 0.0;
    int m_animationDuration = 200;

    // 尺寸相关
    QRectF m_trackRect;
    QRectF m_thumbRect;
    int m_trackHeight = 24;
    int m_trackWidth = 52;
    int m_thumbRadius = 12;
    qreal m_trackRadius = 12.0;

    // 阴影效果
    QColor m_shadowColor = QColor(0, 0, 0, 30);
    qreal m_shadowBlur = 5.0;
    QPointF m_shadowOffset = QPointF(0, 1);
};