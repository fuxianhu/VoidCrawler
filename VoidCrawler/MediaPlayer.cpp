#include "MediaPlayer.h"
#include "Core.h"
#include "SmoothSwitch.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QGuiApplication>
#include <QScreen>
#include <QElapsedTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QBackingStore>
#include <QFileDialog>
#include <QFileInfo>
#include <QString>
#include <QDebug>
#include <QAudioOutput>
#include <QVBoxLayout>
#include <QMimeData>
#include <QSlider>
#include <QWidget>
#include <QSpinBox>

#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

AudioPlayer::AudioPlayer(QWidget* parent)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground, false);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_AlwaysShowToolTips, false);

    // 无边框窗口
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    resize(490, 200);

    QWidget* central = new QWidget(this);
    central->setObjectName("centralWidget");
    central->setAttribute(Qt::WA_NoMousePropagation, false);
    setCentralWidget(central);

    setWindowTitle("Audio Player");
    setWindowIcon(QIcon(VCCore::getPath("icon/VoidCrawlerIcon.ico")));
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::transparent);
    setPalette(pal);
    central->setAutoFillBackground(true);
    central->setPalette(pal);

    initUI();
    applyHighPerformanceSettings();
}

void AudioPlayer::on_selectButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select Audio File", QString(),
        "Audio Files (*.mp3 *.wav *.wma);;All Files (*)");
    VCCore::logger->info("Selected audio file: {}", filePath.toStdString());
    if (filePath != "")
    {
        QFileInfo fileInfo(filePath);
        titleLabel->setText(fileInfo.baseName()); // 文件名（不带扩展名）
        player->setSource(QUrl::fromLocalFile(filePath));
        player->play();
    }
}

void AudioPlayer::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty() && urls.first().isLocalFile())
        {
            QString filePath = urls.first().toLocalFile();
            QFileInfo fileInfo(filePath);
            QString ext = fileInfo.suffix().toLower();
            if (ext == "mp3" || ext == "wav" || ext == "wma")
            {
                originalTitle = titleLabel->text();
                titleLabel->setText(fileInfo.baseName());
                event->acceptProposedAction();
                return;
            }
            else
            {
                originalTitle = titleLabel->text();
                titleLabel->setText("Error");
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}

void AudioPlayer::dragLeaveEvent(QDragLeaveEvent* event)
{
    Q_UNUSED(event);
    titleLabel->setText(originalTitle);
}

void AudioPlayer::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty() && urls.first().isLocalFile())
        {
            QString filePath = urls.first().toLocalFile();
            QFileInfo fileInfo(filePath);
            QString ext = fileInfo.suffix().toLower();
            if (ext == "mp3" || ext == "wav" || ext == "wma")
            {
                VCCore::logger->info("Dropped audio file: {}", filePath.toStdString());
                titleLabel->setText(fileInfo.baseName());
                player->setSource(QUrl::fromLocalFile(filePath));
                player->play();
                event->acceptProposedAction();
                return;
            }
            else
            {
                titleLabel->setText("Error");
                QTimer::singleShot(1000, [this]() { titleLabel->setText(originalTitle); });
                event->acceptProposedAction();
                return;
            }
        }
    }
    titleLabel->setText("Error");
    QTimer::singleShot(1000, [this]() { titleLabel->setText(originalTitle); });
    event->ignore();
}

void AudioPlayer::on_playButton_clicked()
{
    player->play();
}

void AudioPlayer::on_pauseButton_clicked()
{
    player->pause();
}

void AudioPlayer::on_stopButton_clicked()
{
    player->stop();
}

void AudioPlayer::initUI()
{
    player = new QMediaPlayer();
    QAudioOutput* audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);

    //QPainterPath PainterPath; // 绘制一个QPainter路径对象
    //PainterPath.addRoundedRect(rect(), 10, 10); // 对它设置路径为四周圆角。10即圆角的深度
    //QRegion mask = QRegion(PainterPath.toFillPolygon().toPolygon()); // 创建遮罩对象，内容即该QPainter路径
    //setMask(mask); // 为QWidget覆盖上这个遮罩

    setAcceptDrops(true);

    QWidget* w = centralWidget();
    w->setAttribute(Qt::WA_TranslucentBackground);

    // 设置所有子部件的透明背景
    auto setWidgetTransparent = [](QWidget* widget)
    {
        widget->setAttribute(Qt::WA_TranslucentBackground);
        widget->setAutoFillBackground(false);
    };

    closeBtn = new QPushButton("✗", w);
    closeBtn->setFixedSize(32, 32);
    closeBtn->move(width() - 32 - 8, 8);
    closeBtn->setStyleSheet(
        "QPushButton{color:#fff;background:transparent;border:none;font-size:20px;border-radius:16px;}"
        "QPushButton:hover{background:#ff4c4c;}"
    );

    connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);

    titleLabel = new QLabel(" >> Audio Player <<", w); // Once upon a time
    titleLabel->move(20, 30);
    titleLabel->setStyleSheet(R"(
        QLabel {
            color: white;
            background: qlineargradient(
                x1:0, y1:0, x2:1, y2:0,
                stop:0 #e0c3fc,
                stop:1 #8ec5fc
            );
            border-radius: 12px;
            padding: 10px 20px;
            font-family: 'Cascadia Code';
            font-size: 24px;
            font-weight: bold;
            border: 2px solid rgba(255, 255, 255, 0.2);
        }
    )");

    selectBtn = new QPushButton("Select Audio", w);
    selectBtn->setFixedSize(100, 30);
    selectBtn->move(10, height() - 40);
    playBtn = new QPushButton("Play", w);
    playBtn->setFixedSize(60, 30);
    playBtn->move(120, height() - 40);
    pauseBtn = new QPushButton("Pause", w);
    pauseBtn->setFixedSize(60, 30);
    pauseBtn->move(190, height() - 40);
    stopBtn = new QPushButton("Stop", w);
    stopBtn->setFixedSize(60, 30);
    stopBtn->move(260, height() - 40);
    connect(selectBtn, &QPushButton::clicked, this, &AudioPlayer::on_selectButton_clicked);
    connect(playBtn, &QPushButton::clicked, this, &AudioPlayer::on_playButton_clicked);
    connect(pauseBtn, &QPushButton::clicked, this, &AudioPlayer::on_pauseButton_clicked);
    connect(stopBtn, &QPushButton::clicked, this, &AudioPlayer::on_stopButton_clicked);


    loopPlayLabel = new QLabel("Loop Play:", w);
    loopPlayLabel->move(330, height() - 30);
    loopPlayLabel->setFont(QFont("Cascadia Code", 12));
    loopPlayLabel->setStyleSheet("color: white;");
    loopPlaySwitch = new SmoothSwitch(this);
    loopPlaySwitch->setChecked(true);
    player->setLoops(QMediaPlayer::Infinite);
    connect(loopPlaySwitch, &SmoothSwitch::toggled, this, &AudioPlayer::onLoopPlaySwitchToggled);
    loopPlaySwitch->move(425, height() - 40);

    QString spinBoxStyle = R"(
        QSpinBox {
            border: 1px solid #bdc3c7;
            border-radius: 4px;
            padding: 5px;
            background: white;
            color: #2c3e50;
        }
        QSpinBox::up-button, QSpinBox::down-button {
            width: 16px;
            border: 1px solid #bdc3c7;
            border-radius: 2px;
            background: #ecf0f1;
        }
        QSpinBox::up-button:hover, QSpinBox::down-button:hover {
            background: #723CC4;
        }
    )";
    pSpinBoxSecond = new QSpinBox(w);
    pSpinBoxSecond->setMinimum(0);
    pSpinBoxSecond->setMaximum(59);
    pSpinBoxSecond->setSingleStep(1);
    pSpinBoxSecond->setSuffix(" 秒");
    pSpinBoxSecond->setStyleSheet(spinBoxStyle);

    pSpinBoxMinute = new QSpinBox(w);
    pSpinBoxMinute->setMinimum(0);
    pSpinBoxMinute->setMaximum(0);
    pSpinBoxMinute->setSingleStep(1);
    pSpinBoxMinute->setSuffix(" 分");
    pSpinBoxMinute->setStyleSheet(spinBoxStyle);

    positionSlider = new QSlider(w);
    positionSlider->setOrientation(Qt::Horizontal);
    positionSlider->setMinimum(0);
    positionSlider->setMaximum(0);
    positionSlider->setSingleStep(1);
    positionSlider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            height: 4px;
            background: qlineargradient(
                x1:0, y1:0, x2:1, y2:0,
                stop:0 #00CED1,      /* 青色 */
                stop:0.5 #9370DB,    /* 中紫色 */
                stop:1 #FF69B4       /* 粉色 */
            );
            border-radius: 2px;
            box-shadow: inset 0 1px 1px rgba(0, 0, 0, 0.3);
        }
    
        /* 已播放部分透明 */
        QSlider::sub-page:horizontal {
            background: transparent;
            border-radius: 2px;
        }
    
        /* 未播放部分 - 暗色蒙版 */
        QSlider::add-page:horizontal {
            background: rgba(20, 20, 20, 0.9);
            border-radius: 2px;
        }
    
        QSlider::handle:horizontal {
            width: 14px;
            height: 14px;
            margin: -5px 0;
            border-radius: 7px;
            background: qradialgradient(
                cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5,
                stop:0 #ffffff,
                stop:0.3 #b28de3,     /* 中间淡紫色 */
                stop:1 #723CC4        /* 外圈紫色 */
            );
            border: 1px solid #8B4789;
            box-shadow: 0 1px 3px rgba(0, 0, 0, 0.5);
        }
    
        QSlider::handle:horizontal:hover {
            width: 16px;
            height: 16px;
            margin: -6px 0;
            border-radius: 8px;
            background: qradialgradient(
                cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5,
                stop:0 #ffffff,
                stop:0.2 #d0b9f0,
                stop:1 #8a5cd6
            );
            border: 1px solid #9B4F96;
            box-shadow: 0 0 8px rgba(255, 110, 180, 0.3);
        }
    
        QSlider::handle:horizontal:pressed {
            background: qradialgradient(
                cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5,
                stop:0 #f0f0f0,
                stop:0.3 #a07cd9,
                stop:1 #5a2d9d
            );
            border: 1px solid #7A378B;
        }
    )");

    auto updateMinuteSecondDisplay = [=](int totalSeconds)
    {
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;

        pSpinBoxMinute->blockSignals(true);
        pSpinBoxSecond->blockSignals(true);

        pSpinBoxMinute->setValue(minutes);
        pSpinBoxSecond->setValue(seconds);

        pSpinBoxMinute->blockSignals(false);
        pSpinBoxSecond->blockSignals(false);
    };

    auto calculateTotalSeconds = [=]() -> int
    {
        return pSpinBoxMinute->value() * 60 + pSpinBoxSecond->value();
    };

    connect(pSpinBoxMinute, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int minuteValue)
    {
        int totalSeconds = calculateTotalSeconds();
        positionSlider->blockSignals(true);
        positionSlider->setValue(totalSeconds);
        positionSlider->blockSignals(false);

        if (totalSeconds > positionSlider->maximum())
        {
            int maxSeconds = positionSlider->maximum();
            int maxMinutes = maxSeconds / 60;
            int maxRemainingSeconds = maxSeconds % 60;

            pSpinBoxMinute->blockSignals(true);
            pSpinBoxSecond->blockSignals(true);

            pSpinBoxMinute->setValue(maxMinutes);
            pSpinBoxSecond->setValue(maxRemainingSeconds);

            pSpinBoxMinute->blockSignals(false);
            pSpinBoxSecond->blockSignals(false);

            totalSeconds = maxSeconds;
        }

        player->setPosition(static_cast<qint64>(totalSeconds) * 1000);
    });

    connect(pSpinBoxSecond, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int secondValue)
    {
        if (secondValue >= 60)
        {
            pSpinBoxSecond->blockSignals(true);
            pSpinBoxMinute->blockSignals(true);

            int currentMinutes = pSpinBoxMinute->value();
            int currentSeconds = secondValue;

            int additionalMinutes = currentSeconds / 60;
            int remainingSeconds = currentSeconds % 60;

            pSpinBoxMinute->setValue(currentMinutes + additionalMinutes);
            pSpinBoxSecond->setValue(remainingSeconds);

            pSpinBoxMinute->blockSignals(false);
            pSpinBoxSecond->blockSignals(false);
        }

        int totalSeconds = calculateTotalSeconds();
        positionSlider->blockSignals(true);
        positionSlider->setValue(totalSeconds);
        positionSlider->blockSignals(false);

        if (totalSeconds > positionSlider->maximum())
        {
            int maxSeconds = positionSlider->maximum();
            int maxMinutes = maxSeconds / 60;
            int maxRemainingSeconds = maxSeconds % 60;

            pSpinBoxMinute->blockSignals(true);
            pSpinBoxSecond->blockSignals(true);

            pSpinBoxMinute->setValue(maxMinutes);
            pSpinBoxSecond->setValue(maxRemainingSeconds);

            pSpinBoxMinute->blockSignals(false);
            pSpinBoxSecond->blockSignals(false);

            totalSeconds = maxSeconds;
        }

        player->setPosition(static_cast<qint64>(totalSeconds) * 1000);
    });

    connect(positionSlider, &QSlider::valueChanged, this, [=](int totalSeconds)
    {
        updateMinuteSecondDisplay(totalSeconds);
    });

    connect(player, &QMediaPlayer::durationChanged, this, [=](qint64 duration)
    {
        if (duration > 0)
        {
            int totalSeconds = duration / 1000;
            positionSlider->setMaximum(totalSeconds);

            int maxMinutes = totalSeconds / 60;
            pSpinBoxMinute->setMaximum(maxMinutes);
        }
    });

    connect(player, &QMediaPlayer::positionChanged, this, [=](qint64 position)
    {
        if (!positionSlider->isSliderDown())
        {
            int totalSeconds = position / 1000;

            positionSlider->blockSignals(true);
            positionSlider->setValue(totalSeconds);
            positionSlider->blockSignals(false);

            updateMinuteSecondDisplay(totalSeconds);
        }
    });
    connect(positionSlider, &QSlider::valueChanged, this, [=]()
    {
        int totalSeconds = positionSlider->value();
        player->setPosition(static_cast<qint64>(totalSeconds) * 1000);
        updateMinuteSecondDisplay(totalSeconds);
    });

    pSpinBoxMinute->move(15, 120);
    pSpinBoxMinute->setFixedWidth(70);

    pSpinBoxSecond->move(90, 120);
    pSpinBoxSecond->setFixedWidth(70);

    positionSlider->move(175, 120);
    positionSlider->setFixedWidth(300);
    positionSlider->setFixedHeight(20);

    pSpinBoxMinute->setValue(0);
    pSpinBoxSecond->setValue(0);
    positionSlider->setValue(0);

    acryBackground = QColor(240, 240, 240, 150);
    acryOpacity = 50;
    hwnd = HWND(winId());
    huser = GetModuleHandle(L"user32.dll");

    // Aero效果
    //if (huser)
    // {
    //    setWindowCompositionAttribute = (pfnSetWindowCompositionAttribute)GetProcAddress(huser, "SetWindowCompositionAttribute");
    //    if (setWindowCompositionAttribute)
    // {
    //        //DWORD gradientColor = DWORD(0x50FFFFFF);
    //        ACCENT_POLICY accent = { ACCENT_ENABLE_BLURBEHIND, 0, 0, 0 };
    //        WINDOWCOMPOSITIONATTRIBDATA data;
    //        data.Attrib = WCA_ACCENT_POLICY;
    //        data.pvData = &accent;
    //        data.cbData = sizeof(accent);
    //        setWindowCompositionAttribute(hwnd, &data);
    //    }
    //}

    // 亚克力效果
    if (huser)
    {
        setWindowCompositionAttribute = (pfnSetWindowCompositionAttribute)GetProcAddress(huser, "SetWindowCompositionAttribute");
        if (setWindowCompositionAttribute)
        {
            DWORD gradientColor = DWORD(0x50F5F5F5);
            ACCENT_POLICY accent = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, gradientColor, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data;
            data.Attrib = WCA_ACCENT_POLICY;
            data.pvData = &accent;
            data.cbData = sizeof(accent);
            setWindowCompositionAttribute(hwnd, &data);
        }
    }
}

void AudioPlayer::onLoopPlaySwitchToggled(bool checked)
{
    if (checked)
    {
        player->setLoops(QMediaPlayer::Infinite);
    }
    else
    {
        player->setLoops(QMediaPlayer::Once);
    }
}

void AudioPlayer::applyHighPerformanceSettings()
{
    setUpdatesEnabled(true);
#ifdef Q_OS_WIN
    HWND hwnd = (HWND)winId();
    BOOL compositionEnabled;
    if (DwmIsCompositionEnabled(&compositionEnabled) == S_OK && compositionEnabled)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }
#endif
    setAttribute(Qt::WA_AcceptTouchEvents, false);
}

void AudioPlayer::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QWidget* child = childAt(event->pos());
        if (!child || child == centralWidget() ||
            (child != closeBtn && child != titleLabel))
        {

            isDragging = true;
            dragStartPosition = event->globalPosition().toPoint();
            windowStartPosition = pos();
            grabMouse();

            QPoint delta = event->globalPosition().toPoint() - dragStartPosition;
            QPoint newPos = windowStartPosition + delta;
            QRect screenRect = QGuiApplication::primaryScreen()->availableGeometry();
            QRect windowRect = QRect(newPos, size());

            if (windowRect.left() < screenRect.left())
            {
                newPos.setX(screenRect.left());
            }
            if (windowRect.top() < screenRect.top())
            {
                newPos.setY(screenRect.top());
            }
            if (windowRect.right() > screenRect.right())
            {
                newPos.setX(screenRect.right() - width());
            }
            if (windowRect.bottom() > screenRect.bottom())
            {
                newPos.setY(screenRect.bottom() - height());
            }
            move(newPos);
            event->accept();
            return;
        }
    }

    QMainWindow::mousePressEvent(event);
}

void AudioPlayer::mouseMoveEvent(QMouseEvent* event)
{
    if (isDragging && (event->buttons() & Qt::LeftButton))
    {
        QPoint delta = event->globalPosition().toPoint() - dragStartPosition;
        QPoint newPos = windowStartPosition + delta;
        QRect screenRect = QGuiApplication::primaryScreen()->availableGeometry();
        QRect windowRect = QRect(newPos, size());

        if (windowRect.left() < screenRect.left())
        {
            newPos.setX(screenRect.left());
        }
        if (windowRect.top() < screenRect.top())
        {
            newPos.setY(screenRect.top());
        }
        if (windowRect.right() > screenRect.right())
        {
            newPos.setX(screenRect.right() - width());
        }
        if (windowRect.bottom() > screenRect.bottom())
        {
            newPos.setY(screenRect.bottom() - height());
        }

        move(newPos);
        event->accept();
    }
    else
    {
        QMainWindow::mouseReleaseEvent(event);
    }
}
void AudioPlayer::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isDragging)
    {
        isDragging = false;
        releaseMouse();
        event->accept();
    }
    else
    {
        QMainWindow::mouseReleaseEvent(event);
    }
}

void AudioPlayer::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // 绘制带圆角的半透明背景
    QPainterPath path;
    path.addRoundedRect(rect(), 0, 0);

    // 使用半透明颜色
    painter.fillPath(path, QColor(0, 0, 0, 10));
}

void AudioPlayer::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    applyHighPerformanceSettings();
}

AudioPlayer::~AudioPlayer()
{
    delete player;
}