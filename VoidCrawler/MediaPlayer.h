#pragma once

#include "SmoothSwitch.h"
#include "WindowCompositionAttribute.h"

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
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;

    void dragEnterEvent(QDragEnterEvent* event); // 进入窗口
	void dragLeaveEvent(QDragLeaveEvent* event); // 离开窗口
    void dropEvent(QDropEvent* event); // 放置

private:
    bool isDragging = false;
    QPoint dragStartPosition;
    QPoint windowStartPosition;

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

private:
    void initUI();

    void on_selectButton_clicked();
    void on_playButton_clicked();
    void on_pauseButton_clicked();
    void on_stopButton_clicked();
    void onLoopPlaySwitchToggled(bool checked);

    void applyHighPerformanceSettings();

    HWND hwnd;
    HMODULE huser;
    pfnSetWindowCompositionAttribute setWindowCompositionAttribute;

    QColor acryBackground;
    int acryOpacity;
};