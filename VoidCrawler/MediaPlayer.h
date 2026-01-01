#pragma once

#include "SmoothSwitch.h"

#include <QMainWindow>
#include <QMediaPlayer>
#include <QLabel>
#include <QPushButton>
#include <QPoint>
#include <QTimer>
#include <QSlider>
#include <QSpinBox>

class AudioPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit AudioPlayer(QWidget* parent = nullptr);
    ~AudioPlayer();

protected:
    // 重写鼠标事件实现最流畅拖拽
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    // 重写绘制事件实现高效圆角
    void paintEvent(QPaintEvent* event) override;

    // 优化性能
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

    void dragEnterEvent(QDragEnterEvent* event); // 拖动文件进入窗口时触发的事件处理
	void dragLeaveEvent(QDragLeaveEvent* event); // 拖动文件离开窗口时触发的事件处理
    void dropEvent(QDropEvent* event); // 拖动文件在窗口上放置时触发的事件处理

private:
    // 高性能拖拽相关
    bool isDragging = false;
    QPoint dragStartPosition;
    QPoint windowStartPosition;

    // 平滑移动相关
    QTimer* smoothMoveTimer;
    QPoint targetPosition;
    QPoint moveStartPosition;
    qint64 moveStartTime;

    // UI组件
    QMediaPlayer* player = nullptr;
    QLabel* titleLabel = nullptr;
    QPushButton* closeBtn = nullptr;
	QPushButton* selectBtn = nullptr;
	QPushButton* playBtn = nullptr;
	QPushButton* pauseBtn = nullptr;
	QPushButton* stopBtn = nullptr;
    SmoothSwitch* loopPlaySwitch = nullptr;
    QLabel* loopPlayLabel = nullptr;
    QSlider* positionSlider = nullptr;
    QSpinBox* pSpinBoxSecond = nullptr;
    QSpinBox* pSpinBoxMinute = nullptr;

    bool loopPlay = true;
    QString originalTitle;

    // 性能优化标志
    bool highPerformanceMode = false;

private slots:
    void onSmoothMoveTimeout();

private:
    void initUI();

    void on_selectButton_clicked();
    void on_playButton_clicked();
    void on_pauseButton_clicked();
    void on_stopButton_clicked();
    void onLoopPlaySwitchToggled(bool checked);

    void initSmoothMove();
    void applyHighPerformanceSettings();
    void updatePositionSmoothly(const QPoint& targetPos);
};