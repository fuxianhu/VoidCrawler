#include "SmoothSwitch.h"
#include <QPainterPath>
#include <QTimer>
#include <QGraphicsDropShadowEffect>

SmoothSwitch::SmoothSwitch(QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(m_trackWidth + 10, m_trackHeight + 10);
    setCursor(Qt::PointingHandCursor);

    // 初始化动画
    setupAnimations();

    // 初始化位置
    updateSwitchGeometry();
    m_thumbPosition = m_checked ? 1.0 : 0.0;

    // 可选：添加阴影效果
    auto shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(5);
    shadowEffect->setColor(QColor(0, 0, 0, 30));
    shadowEffect->setOffset(0, 1);
    this->setGraphicsEffect(shadowEffect);
}

void SmoothSwitch::setupAnimations()
{
    // 滑块位置动画
    m_thumbAnimation = new QPropertyAnimation(this, "thumbPosition");
    m_thumbAnimation->setDuration(m_animationDuration);
    m_thumbAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    // 轨道颜色动画
    m_trackColorAnimation = new QPropertyAnimation(this, "trackColor");
    m_trackColorAnimation->setDuration(m_animationDuration);
    m_trackColorAnimation->setEasingCurve(QEasingCurve::InOutQuad);
}

void SmoothSwitch::updateSwitchGeometry()
{
    int padding = 5;
    m_trackRect = QRectF(padding, padding, m_trackWidth, m_trackHeight);

    // 滑块位置
    qreal x = padding + (m_thumbPosition * (m_trackWidth - 2 * m_thumbRadius));
    m_thumbRect = QRectF(x, padding, 2 * m_thumbRadius, 2 * m_thumbRadius);
}

bool SmoothSwitch::isChecked() const
{
    return m_checked;
}

void SmoothSwitch::setChecked(bool checked)
{
    if (m_checked == checked)
        return;

    m_checked = checked;

    // 停止正在进行的动画
    m_thumbAnimation->stop();
    m_trackColorAnimation->stop();

    // 设置动画目标值
    qreal targetPosition = m_checked ? 1.0 : 0.0;
    QColor targetColor = m_checked ? m_activeTrackColor : m_trackColor;

    // 开始位置动画
    m_thumbAnimation->setStartValue(m_thumbPosition);
    m_thumbAnimation->setEndValue(targetPosition);
    m_thumbAnimation->start();

    // 开始颜色动画（可选）
    if (m_trackColorAnimation->parent())
    {
        m_trackColorAnimation->setStartValue(m_trackColor);
        m_trackColorAnimation->setEndValue(targetColor);
        m_trackColorAnimation->start();
    }

    // 更新UI
    update();

    // 发射信号
    emit toggled(m_checked);
}

qreal SmoothSwitch::thumbPosition() const
{
    return m_thumbPosition;
}

void SmoothSwitch::setThumbPosition(qreal position)
{
    m_thumbPosition = position;
    updateSwitchGeometry();
    update();
}

QColor SmoothSwitch::trackColor() const
{
    return m_trackColor;
}

void SmoothSwitch::setTrackColor(const QColor& color)
{
    m_trackColor = color;
    update();
}

QColor SmoothSwitch::thumbColor() const
{
    return m_thumbColor;
}

void SmoothSwitch::setThumbColor(const QColor& color)
{
    m_thumbColor = color;
    update();
}

QColor SmoothSwitch::activeTrackColor() const
{
    return m_activeTrackColor;
}

void SmoothSwitch::setActiveTrackColor(const QColor& color)
{
    m_activeTrackColor = color;
    update();
}

void SmoothSwitch::setAnimationDuration(int duration)
{
    m_animationDuration = duration;
    m_thumbAnimation->setDuration(duration);
    m_trackColorAnimation->setDuration(duration);
}

int SmoothSwitch::animationDuration() const
{
    return m_animationDuration;
}

void SmoothSwitch::setSwitchSize(int width, int height)
{
    m_trackWidth = width;
    m_trackHeight = height;
    m_thumbRadius = height / 2 - 2;
    m_trackRadius = height / 2.0;

    setFixedSize(width + 10, height + 10);
    updateSwitchGeometry();
    update();
}

QSize SmoothSwitch::switchSize() const
{
    return QSize(m_trackWidth, m_trackHeight);
}

void SmoothSwitch::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 计算当前轨道颜色（考虑悬停和按下状态）
    QColor currentTrackColor;
    if (m_checked) {
        currentTrackColor = m_activeTrackColor;
        if (m_hovered) currentTrackColor = m_activeHoverColor;
        if (m_pressed) currentTrackColor = m_activeTrackColor.darker(110);
    }
    else {
        currentTrackColor = m_trackColor;
        if (m_hovered) currentTrackColor = m_hoverColor;
        if (m_pressed) currentTrackColor = m_trackColor.darker(110);
    }

    // 绘制轨道
    painter.setPen(Qt::NoPen);
    painter.setBrush(currentTrackColor);
    painter.drawRoundedRect(m_trackRect, m_trackRadius, m_trackRadius);

    // 绘制滑块
    QPainterPath thumbPath;
    thumbPath.addEllipse(m_thumbRect);

    // 滑块渐变效果
    QRadialGradient gradient(m_thumbRect.center(), m_thumbRadius);
    gradient.setColorAt(0, m_thumbColor.lighter(110));
    gradient.setColorAt(0.8, m_thumbColor);
    gradient.setColorAt(1, m_thumbColor.darker(110));

    painter.setBrush(gradient);

    // 滑块阴影
    painter.setPen(QPen(m_shadowColor, 0.5));
    painter.drawPath(thumbPath);

    // 可选：绘制开关状态文本
    if (width() > 100) {  // 只在足够宽时显示文本
        painter.setPen(m_checked ? Qt::white : Qt::gray);
        QFont font = painter.font();
        font.setPointSize(8);
        painter.setFont(font);

        QString text = m_checked ? tr("ON") : tr("OFF");
        QRectF textRect = m_trackRect;
        textRect.setLeft(m_thumbRect.right() + 5);

        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
    }
}

void SmoothSwitch::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        update();
        event->accept();
    }
}

void SmoothSwitch::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_pressed) {
        m_pressed = false;

        if (rect().contains(event->pos())) {
            setChecked(!m_checked);
            emit clicked();
        }

        update();
        event->accept();
    }
}

void SmoothSwitch::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    updateSwitchGeometry();
}

void SmoothSwitch::enterEvent(QEnterEvent* event)
{
    Q_UNUSED(event);
    m_hovered = true;
    update();
}

void SmoothSwitch::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    m_hovered = false;
    update();
}