/*
一个不影响功能的 Bug:
UpdateLayeredWindowIndirect failed for ptDst=(1620, 294), size=(600x1500), dirty=(660x1560 -30, -30) (参数错误。)
这个错误在closeAnimated函数执行前就输出了
由于不影响功能且有关Windows API，不予修复
*/

#include "PropertyWindow.h"
#include "SmoothSwitch.h"
#include "Core.h"
#include "Types.h"

#include <windows.h>
#include "WindowCompositionAttribute.h"
#include <QPainter>
#include <QPainterPath>
#include <QRegion>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QCloseEvent>
#include <QShowEvent>
#include <QScreen>
#include <QApplication>
#include <QLabel>


PropertyWindow::PropertyWindow(QWidget* parent, const QString& itemID)
    : QWidget(parent), itemID(itemID), showAnimation(nullptr), closeAnimation(nullptr)
{
    if (itemID.isEmpty())
    {
        VCCore::logger->error("PropertyWindow::PropertyWindow() Error: item ID arg is empty!");
        initState = false;
        return;
    }
    if (!VCCore::itemConfiguration.object().contains(itemID))
    {
        // 部分功能没有属性，警告即可
        VCCore::logger->warn("PropertyWindow::PropertyWindow() Error: item ID Not Found!");
        initState = false;
        return;
    }
    const QJsonObject obj = VCCore::itemConfiguration.object().value(itemID).toObject();
    if (obj.size() == 0)
    {
        VCCore::logger->error("PropertyWindow::PropertyWindow() Error: item ID has no properties!");
        initState = false;
        return;
	}
    // 迭代器
    quint32 maxWidth = 0, maxHeight = obj.size() * 100;
    const QString title = "Properties - " + itemID;
    for (auto it = obj.begin(); it != obj.end(); it++)
    {
        QString key = it.key();
        QJsonValue value = it.value();
		maxWidth = max(maxWidth, key.length() * 20 + 200);
    }
    maxWidth = max(max(200, maxWidth), title.length() * 10) + 20;

    this->setWindowTitle(title);
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->resize(maxWidth, maxHeight);
    this->setAttribute(Qt::WA_TranslucentBackground);

    auto shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(20);
    shadowEffect->setOffset(0, 0);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    this->setGraphicsEffect(shadowEffect);

    titleLabel = new QLabel(this);
	titleLabel->setText(title);
	titleLabel->setGraphicsEffect(shadowEffect);

    closeBtn = new QPushButton("✗", this);
    closeBtn->setStyleSheet(
        "QPushButton{color:#fff;background:transparent;border:none;font-size:20px;border-radius:16px;}"
        "QPushButton:hover{color:#121212;background:#ff4c4c;}"
    );
    closeBtn->setFixedSize(32, 32);
    closeBtn->move(width() - 32 - 8, 8);

    connect(closeBtn, &QPushButton::clicked, this, &PropertyWindow::closeAnimated);

    for (auto it = obj.begin(); it != obj.end(); it++)
    {
        if (!it.value().isObject())
        {
            VCCore::logger->error("PropertyWindow::PropertyWindow() Error: JSON File Format Error!");
            initState = false;
            return;
        }
        QString nowType = it.value().toObject().value("nowType").toString();
        //QJsonArray types = it.value().toObject().value("type").toArray();
        //for (auto i : types)
        //{
        //    //if (i.toString() == "String")
        //}
        //{
      //  if (value.isNull())
      //  {
      //  }
      //  else if (value.isUndefined())
      //  {
      //  }
      //  else if (value.isBool())
      //  {
      //  }
      //  else if (value.isDouble())
      //  {
      //  }
      //  else if (value.isString())
      //  {
   	  //  }
      //  else if (value.isArray())
      //  {
      //  }
      //  else if (value.isObject())
      //  {
      //  }
    }
    initState = true;
}

PropertyWindow::~PropertyWindow()
{
    if (showAnimation)
    {
        showAnimation->deleteLater();
    }
    if (closeAnimation)
    {
        closeAnimation->deleteLater();
    }
}

void PropertyWindow::setupAnimations()
{
    // 确保只初始化一次
    if (showAnimation || closeAnimation)
    {
        return;
    }

    showAnimation = new QPropertyAnimation(this, "windowPosition");
    showAnimation->setDuration(300);
    showAnimation->setEasingCurve(QEasingCurve::OutCubic);

    closeAnimation = new QPropertyAnimation(this, "windowPosition");
    closeAnimation->setDuration(300);
    closeAnimation->setEasingCurve(QEasingCurve::InCubic);

    connect(closeAnimation, &QPropertyAnimation::finished,
        this, &PropertyWindow::onCloseAnimationFinished);
}

void PropertyWindow::showAnimated()
{
    // 延迟初始化动画
    setupAnimations();

    // 获取屏幕信息
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();

    // 计算最终位置（屏幕中央）
    int finalX = (screenGeometry.width() - width()) / 2;
    int finalY = (screenGeometry.height() - height()) / 2;

    // 设置初始位置（屏幕底部中央）
    int startX = finalX;
    int startY = screenGeometry.bottom() + 10; // 稍微超出屏幕一点

    // 先移动到初始位置（隐藏状态）
    move(startX, startY);

    show();

    QApplication::processEvents();

    showAnimation->setStartValue(QPoint(startX, startY));
    showAnimation->setEndValue(QPoint(finalX, finalY));

    showAnimation->start();
}

void PropertyWindow::closeAnimated()
{
    setupAnimations();

    if (closeAnimation->state() == QAbstractAnimation::Running)
    {
        return;
    }

    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();

    int finalX = (screenGeometry.width() - width()) / 2;
    int finalY = screenGeometry.bottom() + 10;

    QPoint currentPos = pos();
    QPoint endPos(finalX, finalY);

    closeAnimation->setStartValue(currentPos);
    closeAnimation->setEndValue(endPos);
    closeAnimation->start();
}

void PropertyWindow::onCloseAnimationFinished()
{
    QWidget::close();
}

void PropertyWindow::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    // 如果是第一次显示，使用动画
    if (isFirstShow)
    {
        isFirstShow = false;

        // 延迟一点时间确保窗口已经初始化完成
        QTimer::singleShot(10, this, [this]() {
            if (!showAnimation || showAnimation->state() != QAbstractAnimation::Running)
            {
                startShowAnimation();
            }
        });
    }
}

void PropertyWindow::closeEvent(QCloseEvent* event)
{
    setupAnimations();

    if (closeAnimation->state() == QAbstractAnimation::Running)
    {
        event->ignore();
        return;
    }

    if (event->spontaneous())
    {
        event->ignore();
        closeAnimated();
        return;
    }

    QWidget::closeEvent(event);
}

void PropertyWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor("#121212")));
    painter.drawRoundedRect(rect().adjusted(5, 5, -5, -5), 15, 15);
}

void PropertyWindow::startShowAnimation()
{
    setupAnimations();

    if (showAnimation->state() == QAbstractAnimation::Running)
    {
        return;
    }

    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();

    int finalX = (screenGeometry.width() - width()) / 2;
    int finalY = (screenGeometry.height() - height()) / 2;

    int startX = finalX;
    int startY = screenGeometry.bottom() + 10;

    // 如果当前不在起始位置，先移动到起始位置
    if (pos() != QPoint(startX, startY))
    {
        move(startX, startY);
        QApplication::processEvents(); // 确保移动完成
    }

    showAnimation->setStartValue(QPoint(startX, startY));
    showAnimation->setEndValue(QPoint(finalX, finalY));

    showAnimation->start();
}