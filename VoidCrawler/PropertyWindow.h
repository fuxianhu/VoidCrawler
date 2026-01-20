#pragma once
#include <QMainWindow>
#include <QPushButton>
#include "windows.h"
#include "SmoothSwitch.h"
#include "WindowCompositionAttribute.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QTimer>


class PropertyWindow : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QPoint windowPosition READ pos WRITE move) // 改为windowPosition
    
public:
    PropertyWindow(QWidget* parent = nullptr, const QString& itemID = "");
    ~PropertyWindow();

    void showAnimated();
    void closeAnimated();

protected:
    void paintEvent(QPaintEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;

private slots:
    void onCloseAnimationFinished();

private:
    QPushButton* closeBtn;
    QPropertyAnimation* showAnimation;
    QPropertyAnimation* closeAnimation;
    QString itemID;
    bool isFirstShow = true; // 添加标志判断是否是第一次显示

    void setupAnimations();
    void startShowAnimation();
};
