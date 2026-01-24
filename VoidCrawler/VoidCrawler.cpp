#include "VoidCrawler.h"
#include "Core.h"
#include "ui_VoidCrawler.h"
#include "Encryption.h"
#include "configOperate.h"
#include "LicenseAgreementDialog.h"
#include "PrivilegeLevel.h"
#include "FunctionSet.h"
#include "keyhookthread.h"
#include "MediaPlayer.h"
#include "PropertyWindow.h"

#include <format>
#include <string>

#include <windows.h>
#include <shellapi.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QSharedMemory>
#include <QWidget>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsBlurEffect>
#include <QDir>
#include <QtWidgets/QPushButton>
#include <QFont>
#include <QtCore>
#include <QBitmap>
#include <QPainter>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QPixmap>
#include <QVariantAnimation>
#include <QLabel>
#include <QtWidgets/QMainWindow>
#include <QPoint>
#include <QTranslator>
#include <QInputDialog>
#include <QEvent>
#include <QSplashScreen>
#include <QKeySequenceEdit>
#include <QTextEdit>
#include <QDialog>
#include <QMessageBox>
#include <QDebug>
#include <QAudioOutput>

// 小型的 QPushButton 子类，支持用于动画的 'rotation' 属性
// 注意：此处没有使用 Q_OBJECT 宏以避免需要 moc 处理；旋转动画通过 QVariantAnimation 实现
class RotatableIconButton : public QPushButton
{
public:
    // 构造函数：初始化父类并将旋转角度设为 0
    RotatableIconButton(QWidget* parent = nullptr) : QPushButton(parent), m_rotation(0) {}
    // 返回当前旋转角度
    qreal rotation() const { return m_rotation; }
    // 设置旋转角度并触发重绘
    void setRotation(qreal r)
    {
        m_rotation = r;
        update();
    }

protected:
    // 自定义绘制：先平移到控件中心，旋转画布，然后绘制图标或委托给基类绘制
    void paintEvent(QPaintEvent* event) override
    {
        Q_UNUSED(event)
            QPainter p(this);
        p.setRenderHint(QPainter::SmoothPixmapTransform);
        p.setRenderHint(QPainter::Antialiasing);
        // 将坐标系移动到控件中心，方便按中心旋转
        p.translate(width() / 2.0, height() / 2.0);
        p.rotate(m_rotation);
        p.translate(-width() / 2.0, -height() / 2.0);
        if (!icon().isNull())
        {
            QSize sz = iconSize();
            QRect r((width() - sz.width()) / 2, (height() - sz.height()) / 2, sz.width(), sz.height());
            QPixmap pm = icon().pixmap(sz);
            p.drawPixmap(r, pm);
        }
        else
        {
            // 如果没有图标，则退回到基类绘制（例如绘制文本）
            QPushButton::paintEvent(event);
        }
    }

private:
    qreal m_rotation; // 存储旋转角度（以度为单位）
};

VoidCrawler::VoidCrawler(QWidget* parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    // 初始化多语言翻译器对象（QTranslator）
    translator = new QTranslator(this);

    // 这里可以在后续调用 initUI() 时进一步构建界面
}

void VoidCrawler::clientLock()
{
    QJsonObject clientLock = VCCore::mainConfig.object().value("clientLock").toObject();
    if (clientLock.value("disableClient").toBool())
    {
        VCCore::logger->info("The client is locked, exiting...");
        std::exit(0);
    }
    const int maxAtt = clientLock.value("maximumAttempts").toInt();
    if (clientLock.value("password").toString() != "" && clientLock.value("salted").toString() != "")
    {
        for (int i = 1; i <= maxAtt; i++)
        {
            bool ok = false;
            QString inputPassword = QInputDialog::getText(
                this,
                tr("VoidCrawler Client Lock"),
                tr("Please enter the client unlock password:"),
                QLineEdit::Password,
                "",
                &ok);
            if (ok && !inputPassword.isEmpty())
            {
                if (EncryptionModel::VerifySaltedSHA256(
                    clientLock.value("password").toString(),
                    inputPassword,
                    clientLock.value("salted").toString()))
                {
                    VCCore::logger->info("Client unlocked successfully.");
                    return;
                }
                else
                {
                    if (maxAtt == i)
                    {
                        QMessageBox::critical(
                            this,
                            tr("VoidCrawler Client Lock"),
                            tr("The number of attempts has reached the limit, and your operation has been recorded!")
                            // 尝试次数已达到限制，您的操作已被记录！
                        );
                        // 写入 json，锁定客户端
                        QJsonObject root = VCCore::mainConfig.object();
                        QJsonObject clientLockObj = root["clientLock"].toObject();
                        clientLockObj["disableClient"] = true;
                        root["clientLock"] = clientLockObj;
                        VCCore::mainConfig.setObject(root);
                        writeJSON(VCCore::mainConfig, MAIN_JSON_FILE);
                        std::exit(0);
                    }
                    else
                    {
                        QMessageBox::critical(
                            this,
                            tr("VoidCrawler Client Lock"),
                            tr("Password error, please enter again!"));
                    }
                }
            }
            else
            {
                VCCore::logger->info("The user exited during the unlocking process.");
                std::exit(0);
            }
        }
        VCCore::logger->warn("The user has tried multiple times but still cannot unlock!");
        std::exit(0);
    }
}

void VoidCrawler::checkConfigFiles()
{
    if (VCCore::Version::fromString(VCCore::mainConfig.object().value("configVersion").toString()) != VCCore::VoidCrawlerVersion)
    {
        auto result = QMessageBox::warning(
            this,
            "VoidCrawler",
            QString::fromStdString(
                tr("检测到配置文件版本与客户端版本不一致，这可能会导致严重异常，建议按“Ok”立即关闭客户端并手动修复！\n客户端版本：").toStdString() +
                VCCore::VoidCrawlerVersion.toStdString() +
                tr("\n配置文件版本：").toStdString() +
                VCCore::mainConfig.object().value("configVersion").toString().toStdString() +
                "\n"),
            QMessageBox::Ignore | QMessageBox::Ok,
            QMessageBox::NoButton);
        if (result == QMessageBox::Ok)
        {
            std::exit(0);
        }
        else if (result == QMessageBox::Ignore)
        {
            VCCore::logger->warn("Configuration version mismatch ignored by user.");
        }
        else
        {
            VCCore::logger->info("The user closed the dialog box and exited the client ..");
            std::exit(0);
        }
    }
}

void VoidCrawler::agreeTerms()
{
    if (VCCore::mainConfig.object().value("agreeTerms").toBool() == false)
    {
        LicenseAgreementDialog dialog;
        if (dialog.exec() == QDialog::Accepted && dialog.isLicenseAccepted())
        {
            // 用户同意条款，更新配置文件
            VCCore::logger->info("User has accepted the license agreement.");
            QJsonObject root = VCCore::mainConfig.object();
            root["agreeTerms"] = true;
            VCCore::mainConfig.setObject(root);
            writeJSON(VCCore::mainConfig, MAIN_JSON_FILE);
        }
        else
        {
            qApp->exit();
            std::exit(0);
        }
    }
}

void VoidCrawler::multipleApplicationsProcessing()
{
    // 创建命名互斥体
    HANDLE hMutex = CreateMutexW(nullptr, TRUE, L"VoidCrawlerClientUniqueIdentifier");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        if (hMutex)
        {
            CloseHandle(hMutex);
        }
        if (VCCore::mainConfig.object().value("enableMultipleApplications").toBool() == false)
        {
            qApp->exit();
            std::exit(0);
        }
    }
}

void VoidCrawler::privilegeIconDisplay()
{
    const std::map<PrivilegeLevel, std::string> iconMap =
    {
        {PrivilegeLevel::Unknown,
         "icon/User.png"},
        {PrivilegeLevel::StandardUser,
         "icon/User.png"},
        {PrivilegeLevel::Administrator,
         "icon/Administrator.png"},
        {PrivilegeLevel::System,
         "icon/System.png"},
        {PrivilegeLevel::TrustedInstaller,
         "icon/TrustedInstaller.png"},
        {PrivilegeLevel::Other,
         "icon/User.png"} };
    QPixmap pixmap(VCCore::getPath(
        iconMap.at(PrivilegeChecker::GetCurrentPrivilegeLevel())));
    VCCore::logger->info(PrivilegeChecker::GetPrivilegeLevelString(PrivilegeChecker::GetCurrentPrivilegeLevel()));
    pixmap = pixmap.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    privilegeIcon = new QLabel();
    privilegeIcon->setPixmap(pixmap);
    privilegeIcon->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    privilegeIcon->setAttribute(Qt::WA_TranslucentBackground);
    privilegeIcon->move(10, 10);
    privilegeIcon->show();
}
bool isDisplay = true;
bool firstCall = true;
void VoidCrawler::onHotkeyDetected(const QString& message)
{
    // 在主线程中安全地更新
    if (firstCall)
    {
        firstCall = false;
    }
    else
    {
        if (isDisplay)
        {
            isDisplay = false;
            VCCore::hideAllTopLevelWindows();
        }
        else
        {
            isDisplay = true;
            VCCore::showAllTopLevelWindows();
        }
    }
    VCCore::logger->info(std::format("Main threrad: {} Message: {}",
        QString("0x%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()), 16, 16, QLatin1Char('0')).toStdString(),
        message.toStdString()));
}

void VoidCrawler::onHookError(const QString& error)
{
    m_statusLabel->setText("错误: " + error);
    VCCore::logger->error(std::format("Hook error: {}", error.toStdString()));
}

void VoidCrawler::ProgramAfterStartup()
{
    checkConfigFiles();
    clientLock();
    agreeTerms();
    multipleApplicationsProcessing();
    privilegeIconDisplay();
    // 生成密码哈希：
    // auto [a, b] = EncryptionModel::GenerateSaltedSHA256("1145_gg");
    // VCCore::logger->debug(a.toStdString());
    // VCCore::logger->debug(b.toStdString());
    if (!m_hookThread)
    {
        m_hookThread = new KeyHookThread(this);
        connect(m_hookThread, &KeyHookThread::hotkeyDetected,
            this, &VoidCrawler::onHotkeyDetected);
        connect(m_hookThread, &KeyHookThread::hookError,
            this, &VoidCrawler::onHookError);
        connect(m_hookThread, &QThread::finished,
            m_hookThread, &QObject::deleteLater);

        m_hookThread->start();
    }
    QMediaPlayer* player = new QMediaPlayer(this);
    QAudioOutput* audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    player->setSource(VCCore::getPath("audio/startup.mp3"));
    connect(player, &QMediaPlayer::playbackStateChanged,
        [player, audioOutput](QMediaPlayer::PlaybackState state)
        {
            if (state == QMediaPlayer::StoppedState)
            {
                // 先断开连接，防止重复触发
                player->disconnect();
                // 删除音频输出
                audioOutput->deleteLater();
                // 删除播放器
                player->deleteLater();
            }
        });
    player->play();
}

void VoidCrawler::on_button_right_clicked(QString id, QPushButton* qpb)
{
    VCCore::logger->debug(std::format("Button right clicked, ID: {}", id.toStdString()));
    PropertyWindow* p = new PropertyWindow(nullptr, id);
    p->showAnimated(); 
}

void VoidCrawler::on_button_clicked(QString id, QPushButton* qpb, QString styles)
{
	// 由于 styles 参数是值传递，因此在函数内修改 styles 不会影响外部变量，不会出现超长字符串累积的问题
    // 用 startShellDetached 的原因：
    // 不阻塞主线程，能够执行其他 UI 操作，阻塞时间从3秒降至0.02秒左右
    VCCore::logger->debug(std::format("Button clicked, ID: {}", id.toStdString()));

    // 对于 开关类型，反转 switch （开关状态）
    QJsonArray items = VCCore::categoryConfig.array()[0].toObject().value("items").toArray();
    std::bitset<3> IMESwitchState;
    for (int i = 0; i < items.size(); i++)
    {
        QJsonObject obj = items[i].toObject();
        if (obj.contains("id") && obj["id"].toString() == id)
        {
            if (obj.contains("type") && obj["type"].toInt() == 1)
            {
                bool newValue = !obj["switch"].toBool(); // 反转布尔值
                obj["switch"] = newValue;
                items[i] = obj; // 更新回数组
                if (id == "SwitchIMECtrlSpace") { IMESwitchState[2] = newValue; }
                if (id == "SwitchIMEShift") { IMESwitchState[1] = newValue; }
                if (id == "SwitchIMECtrl") { IMESwitchState[0] = newValue; }
                styles = "background: rgba(0, 0, 0, 0.3); border-radius: 8px; ";
                if (newValue)
                {
                    styles = styles.append("color: green; ");
                }
                else
                {
                    styles = styles.append("color: red; ");
                }
                qpb->setStyleSheet(styles);

                QJsonArray arr = VCCore::categoryConfig.array();
                QJsonObject a = arr[0].toObject();
                a["items"] = items;
                arr[0] = a;
                VCCore::categoryConfig.setArray(arr);
                writeJSON(VCCore::categoryConfig, CATEGORY_JSON_FILE);
                break;
            }
        }
    }
    if (id == "cmd")
    {
        VCCore::startShellDetached("cmd.exe");
    }
    else if (id == "NewPowershell")
    {
        VCCore::startShellDetached("pwsh.exe");
    }
    else if (id == "OldPowershell")
    {
        VCCore::startShellDetached("powershell.exe");
    }
    else if (id == "VisualStudioCode")
    {
        VCCore::startShellDetached("code.exe");
    }
    else if (id == "VisualStudio")
    {
        VCCore::startShellDetached("D:\\Program Files\\Microsoft Visual Studio\\18\\Community\\Common7\\IDE\\devenv.exe");
    }
    else if (id == "WindowsTaskManger")
    {
        VCCore::startShellDetached("Taskmgr.exe");
    }
    else if (id == "EdgeBrowser")
    {
        VCCore::startShellDetached("msedge.exe");
    }
    else if (id == "Weixin")
    {
        VCCore::startShellDetached(QString::fromStdString(FunctionSet::GetWeChatInstallPath()));
    }
    else if (id == "WindowsCalc")
    {
		VCCore::startShellDetached("calc.exe");
    }
    else if (id == "OpenUserFolder")
    {
        VCCore::startShellDetached("explorer.exe", "%USERPROFILE%");
    }
    else if (id == "chcp")
    {
        system("chcp 65001");
    }
    else if (id == "Exit")
    {
        QApplication::quit();
        std::exit(0);
    }
    else if (id == "SwitchIMECtrlSpace")
    {
        std::bitset<3> switchKey = FunctionSet::getMicrosoftPinyinSwitchKey();
        switchKey[2] = IMESwitchState[2];
        FunctionSet::modifyMicrosoftPinyinSwitchKey(switchKey);
    }
    else if (id == "SwitchIMEShift")
    {
        std::bitset<3> switchKey = FunctionSet::getMicrosoftPinyinSwitchKey();
        switchKey[1] = IMESwitchState[1];
        FunctionSet::modifyMicrosoftPinyinSwitchKey(switchKey);
    }
    else if (id == "SwitchIMECtrl")
    {
        std::bitset<3> switchKey = FunctionSet::getMicrosoftPinyinSwitchKey();
        switchKey[0] = IMESwitchState[0];
        FunctionSet::modifyMicrosoftPinyinSwitchKey(switchKey);
    }
    else if (id == "AudioPlayer")
    {
        /*
        创建一个子窗口，同样没有标题栏，圆角
        （或许，可以给所有窗口加一个亚克力半透明效果？
        长度大，宽度小，细长（根据实际歌曲调整长宽，切歌时也动态调整，且有动画）
        第一行写名称，大标题，左对齐
        之后照着书上 543 页做一下
        */
		AudioPlayer* player = new AudioPlayer(nullptr);
        player->show();
    }
    else if (id == "OpenMinecraftChinaEdition")
    {
        VCCore::startShellDetached("fevergames://mygame/?gameId=1");
    }
    else if (id == "OpenBjd")
    {
        VCCore::startShellDetached("minecraft://netease?params=6974656D5F343636313333343436373336363137383838345F315F325F38");
    }
    else if (id == "OpenBjdOfficialWebsite")
    {
        VCCore::startShellDetached("https://mcbjd.net/");
    }
    else
    {
        VCCore::logger->warn("In VoidCrawler::on_button_clicked function, id error!");
    }
}

void VoidCrawler::initUI()
{
    ProgramAfterStartup();
    VCCore::logger->debug("initUI called.");
    // 创建中央部件并设置为主窗口的 central widget
    QWidget* w = new QWidget(this);
    w->setObjectName("centralWidget");
    w->setStyleSheet("background: transparent;");
    this->setCentralWidget(w);

    //         不在任务栏显示图标    无边框窗口，以便自定义标题栏样式    置顶窗口
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->resize(300, m_expandedHeight);
    this->setWindowTitle("VoidCrawler Client");
    this->setWindowIcon(QIcon(VCCore::getPath("icon/VoidCrawlerIcon.ico")));
    this->setStyleSheet("QMainWindow { background-color: #121212; }");
    VCCore::logger->debug("Set Title Bar...");
    // 创建自定义标题栏（位于顶部）
    m_titleBar = new QWidget(w);
    m_titleBar->setObjectName("titleBar");
    int titleBarHeight = 48; // 标题栏高度
    m_titleBar->setGeometry(0, 0, this->width(), titleBarHeight);
    // 标题栏使用线性渐变背景并带有圆角样式
    m_titleBar->setStyleSheet(
        "QWidget#titleBar {"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #111111, stop:1 #0b0b0b);"
        "border-radius: 12px;"
        "}");

    // 标题文本（居中显示）
    m_titleLabel = new QLabel(w);
    m_titleLabel->setText("VoidCrawler Client");
    m_titleLabel->setGeometry(0, 0, this->width(), titleBarHeight);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setFont(QFont(
        VCCore::mainCfgValue("font/title/family").toString(),
        VCCore::mainCfgValue("font/title/pointSize").toInt(),
        VCCore::mainCfgValue("font/title/weight").toInt())
    );
    // 半透明背景，阴影更明显
    m_titleLabel->setStyleSheet("color: white; background: rgba(18,18,18,0.3); border-radius: 8px;");

    m_titleShadow = new QGraphicsDropShadowEffect(m_titleLabel);
    m_titleShadow->setBlurRadius(40);
    m_titleShadow->setOffset(0, 0);
    m_titleShadow->setColor(QColor(255, 0, 0, 200)); // 红色阴影
    m_titleLabel->setGraphicsEffect(m_titleShadow);

    // ====== 彩色动态阴影效果：标题 ======
    m_titleShadow = new QGraphicsDropShadowEffect(m_titleLabel);
    m_titleShadow->setBlurRadius(32);
    m_titleShadow->setOffset(0, 0);
    m_titleLabel->setGraphicsEffect(m_titleShadow);
    m_titleShadowAnim = new QVariantAnimation(this);
    m_titleShadowAnim->setStartValue(0.0);
    m_titleShadowAnim->setEndValue(360.0);
    m_titleShadowAnim->setDuration(VCCore::mainConfig.object().value("animationSpeed").toInt());
    m_titleShadowAnim->setLoopCount(-1);
    
    connect(m_titleShadowAnim, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
        // 彩虹色 HSL 动态
        QColor color;
        color.setHslF(static_cast<float>(fmod(value.toDouble() / 360.0, 1.0)), 0.8f, 0.5f);
        m_titleShadow->setColor(color);
    });
    m_titleShadowAnim->start();
    // ====== 彩色动态阴影效果 END ======

    VCCore::logger->debug("Create Toggle Button...");
    // 创建折叠/展开按钮（位于标题栏右侧），使用自定义可旋转按钮类
    m_toggleButton = new RotatableIconButton(w);
    QPixmap ico(VCCore::getPath("icon/favicon.ico"));
    if (!ico.isNull())
    {
        m_toggleButton->setIcon(QIcon(ico));
        m_toggleButton->setIconSize(QSize(20, 20));
    }
    else
    {
        // 如果图标加载失败，直接退出程序（可根据需要改为回退处理）
        exit(1);
    }
    m_toggleButton->setFixedSize(36, 36);
    m_toggleButton->setStyleSheet("background: transparent; border: none; color: white;");

    VCCore::logger->debug("Create contentWidget...");
    // 创建内容容器（这个容器支持折叠和展开）
    m_contentWidget = new QWidget(w);
    m_contentWidget->setStyleSheet("background: transparent;");
    m_contentWidget->setGeometry(0, titleBarHeight, this->width(), m_contentExpandedHeight);
    // 将内容区设置为固定高度策略，以便通过修改 maximumHeight 来动画化高度变化
    m_contentWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // 使用垂直布局管理内容区内的控件
    QVBoxLayout* contentLayout = new QVBoxLayout(m_contentWidget);
    contentLayout->setContentsMargins(12, 12, 12, 12); // 边距
    contentLayout->setSpacing(8);                      // 子控件间距

    QJsonObject classification = VCCore::categoryConfig.array()[0].toObject();
    QJsonArray items = classification.value("items").toArray();
    if (!items.isEmpty())
    {
        int btnIdx = 0;
        for (const QJsonValue& itemVal : items)
        {
            QJsonObject itemObj = itemVal.toObject();
            QString itemID = itemObj.value("id").toString();
            QString itemName = itemID;
            int valueType = itemObj.value("type").toInt();
            QPushButton* qpb = new QPushButton(itemName, m_contentWidget);
            qpb->setProperty("itemID", itemID);  // 设置索引属性
            qpb->setProperty("isItemButton", true);  // 标识这是自定义按钮

            QString styles = "background: rgba(0, 0, 0, 0.3); border-radius: 8px; ";

            std::bitset<3> switchKey = FunctionSet::getMicrosoftPinyinSwitchKey();
            if (
                (itemID == "SwitchIMECtrlSpace" && switchKey[2] != itemObj.value("switch").toBool()) ||
                (itemID == "SwitchIMEShift" && switchKey[1] != itemObj.value("switch").toBool()) ||
                (itemID == "SwitchIMECtrl" && switchKey[0] != itemObj.value("switch").toBool())
                )
            {
                itemObj["switch"] = !itemObj["switch"].toBool();
                items[btnIdx] = itemObj;
                QJsonArray arr = VCCore::categoryConfig.array();
                QJsonObject a = arr[0].toObject();
                a["items"] = items;
                arr[0] = a;
                VCCore::categoryConfig.setArray(arr);
                writeJSON(VCCore::categoryConfig, CATEGORY_JSON_FILE);
            }
            
            if (valueType == 1) // 开关类型
            {
                if (itemObj.value("switch").toBool())
                {
                    // 开
                    styles = styles.append("color: green; ");
                }
                else
                {
                    // 关
                    styles = styles.append("color: red; ");
                }
            }
            else if (valueType == 0) // 按钮类型
            {
                styles = styles.append("color: white; ");
            }
            qpb->setStyleSheet(styles);

            qpb->setFont(
                QFont(
                    VCCore::mainCfgValue("font/item/family").toString(),
                    VCCore::mainCfgValue("font/item/pointSize").toInt(),
                    VCCore::mainCfgValue("font/item/weight").toInt())
            );
            QGraphicsDropShadowEffect* btnShadow = new QGraphicsDropShadowEffect(qpb);
            btnShadow->setBlurRadius(32);
            btnShadow->setOffset(0, 0);
            btnShadow->setColor(QColor(138, 0, 255, 180));
            qpb->setGraphicsEffect(btnShadow);

            qpb->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

            // 使用C++14的广义lambda捕获
            connect(qpb, &QPushButton::clicked, this, [this, itemID = itemID, qpb = qpb, styles = styles]()
                { VoidCrawler::on_button_clicked(itemID, qpb, styles); });
            contentLayout->addWidget(qpb);

            // 为按钮安装事件过滤器，以处理右键点击等
            qpb->installEventFilter(this);

            // ====== 彩色动态阴影效果：按钮 ======
            QGraphicsDropShadowEffect* btnShadowEffect = new QGraphicsDropShadowEffect(qpb);
            btnShadowEffect->setBlurRadius(24);
            btnShadowEffect->setOffset(0, 0);
            qpb->setGraphicsEffect(btnShadowEffect);
            m_buttonShadows.append(btnShadowEffect);
            QVariantAnimation* btnAnim = new QVariantAnimation(this);
            btnAnim->setStartValue(0.0);
            btnAnim->setEndValue(360.0);
            btnAnim->setDuration(VCCore::mainConfig.object().value("animationSpeed").toInt()); // 速度
            btnAnim->setLoopCount(-1);
            connect(btnAnim, &QVariantAnimation::valueChanged, this, [btnShadowEffect, btnIdx](const QVariant& value) {
                QColor color;
                color.setHslF(static_cast<float>(fmod((value.toDouble() + btnIdx * 10) / 360.0, 1.0)), 0.7f, 0.55f);
                btnShadowEffect->setColor(color);
            });
            btnAnim->start();
            m_buttonShadowAnims.append(btnAnim);
            btnIdx++;
            // ====== 彩色动态阴影效果 END ======
        }
    }

    VCCore::logger->debug("calc window size");
    // 计算内容区展开时的高度：将所有子控件的 sizeHint 高度累加，再加上布局间距和边距
    int contentH = 0;
    int itemCount = contentLayout->count();
    for (int i = 0; i < itemCount; ++i)
    {
        QLayoutItem* it = contentLayout->itemAt(i);
        if (it && it->widget())
        {
            contentH += it->widget()->sizeHint().height();
        }
    }
    // 累加子控件间距和边距
    contentH += contentLayout->spacing() * qMax(0, itemCount - 1);
    QMargins margins = contentLayout->contentsMargins();
    contentH += margins.top() + margins.bottom();
    // 如果计算结果为 0，则使用默认值（防止计算异常导致高度为 0）
    if (contentH <= 0)
        contentH = (m_contentExpandedHeight > 0 ? m_contentExpandedHeight : 200);
    m_contentExpandedHeight = contentH;
    // 折叠时的高度为 0（仅显示标题栏）
    m_contentCollapsedHeight = 0;
    // 计算窗口展开与折叠时的整体高度
    m_collapsedHeight = titleBarHeight;
    m_expandedHeight = titleBarHeight + m_contentExpandedHeight;

    // 根据当前 m_expanded 状态初始化内容区最大高度和窗口高度
    if (m_expanded)
    {
        m_contentWidget->setMaximumHeight(m_contentExpandedHeight);
        this->resize(this->width(), m_expandedHeight);
    }
    else
    {
        m_contentWidget->setMaximumHeight(m_contentCollapsedHeight);
        this->resize(this->width(), m_collapsedHeight);
    }

    // 将内容区的几何高度设置为窗口可用高度与最大高度的较小值，确保子控件可见
    int initialContentH = qMin(m_contentWidget->maximumHeight(), this->height() - titleBarHeight);
    if (initialContentH < 0)
    {
        initialContentH = 0;
    }
    m_contentWidget->setGeometry(0, titleBarHeight, this->width(), initialContentH);
    m_contentWidget->show();

    VCCore::logger->debug("toggleButton connect...");
    // 点击折叠按钮时执行的动画：图标旋转 + 窗口高度动画 + 内容区高度动画
    QObject::connect(m_toggleButton, &QPushButton::clicked, [this]()
        {
            VCCore::logger->debug("toggleButton clicked.");
            // 旋转动画使用 QVariantAnimation，角度在 0 到 180 度之间切换
            qreal start = m_expanded ? 180.0 : 0.0;
            qreal end = m_expanded ? 0.0 : 180.0;
            QVariantAnimation* rotAnim = new QVariantAnimation(this);
            rotAnim->setDuration(300);
            rotAnim->setStartValue(start);
            rotAnim->setEndValue(end);
            rotAnim->setEasingCurve(QEasingCurve::InOutCubic);
            connect(rotAnim, &QVariantAnimation::valueChanged, [this](const QVariant& value) {
                if (auto r = dynamic_cast<RotatableIconButton*>(m_toggleButton)) {
                    r->setRotation(value.toReal());
                }
                });
            rotAnim->start(QAbstractAnimation::DeleteWhenStopped);

            // 窗口高度动画：保持左上角位置不变，仅改变高度
            QRect startGeom = this->geometry();
            int targetHeight = m_expanded ? m_collapsedHeight : ((m_titleBar ? m_titleBar->height() : 48) + m_contentExpandedHeight);
            QRect endGeom(startGeom.x(), startGeom.y(), startGeom.width(), targetHeight);
            QPropertyAnimation* geomAnim = new QPropertyAnimation(this, "geometry");
            geomAnim->setDuration(300);
            geomAnim->setStartValue(startGeom);
            geomAnim->setEndValue(endGeom);
            geomAnim->setEasingCurve(QEasingCurve::InOutCubic);
            geomAnim->start(QAbstractAnimation::DeleteWhenStopped);

            // 内容区高度动画：改变 maximumHeight，从而触发行内布局重排
            int startH = m_contentWidget->maximumHeight();
            int titleBarH = (m_titleBar ? m_titleBar->height() : 48);
            int endH = targetHeight - titleBarH;
            if (endH < 0) endH = 0;
            QPropertyAnimation* contentAnim = new QPropertyAnimation(m_contentWidget, "maximumHeight");
            contentAnim->setDuration(300);
            contentAnim->setStartValue(startH);
            contentAnim->setEndValue(endH);
            contentAnim->setEasingCurve(QEasingCurve::InOutCubic);
            contentAnim->start(QAbstractAnimation::DeleteWhenStopped);

            // 切换展开状态（用于下一次点击判断）
            m_expanded = !m_expanded; });

    VCCore::logger->debug("Other operate...");
    // 支持通过拖动标题栏移动窗口：在标题栏相关控件上安装事件过滤器
    m_titleBar->installEventFilter(this);
    m_titleLabel->installEventFilter(this);
    // 不在按钮上安装过滤器，以保证按钮能接收并处理点击事件

    // 在初始化时执行一次 resizeEvent 以应用窗口圆角遮罩
    QResizeEvent ev(this->size(), this->size());
    this->resizeEvent(&ev);
    VCCore::logger->debug("initUI end.");
}

VoidCrawler::~VoidCrawler()
{
    delete translator; // 这里手动 delete 是安全的，因为 translator 的父对象已设置为 this
    if (m_hookThread)
    {
        m_hookThread->stopHook();
        m_hookThread->quit();
        m_hookThread->wait();
    }
}

void VoidCrawler::changeLanguage(const QString& language)
{
    // 卸载当前翻译器
    QApplication::removeTranslator(translator);

    // 尝试加载新的翻译文件并安装
    if (translator->load(QString(":/translations/%1.qm").arg(language)))
    {
        QApplication::installTranslator(translator);
    }
}

void VoidCrawler::resizeEvent(QResizeEvent* event)
{
    // 先调用基类实现以保持默认行为
    QMainWindow::resizeEvent(event);

    // 重新布局标题栏和标题文本的宽度，确保其宽度与窗口匹配
    if (m_titleBar)
    {
        int titleBarHeight = m_titleBar->height();
        m_titleBar->setGeometry(0, 0, this->width(), titleBarHeight);
    }
    if (m_titleLabel)
    {
        int titleBarHeight = m_titleLabel->height();
        m_titleLabel->setGeometry(0, 0, this->width(), titleBarHeight);
    }
    // 重新定位折叠按钮，固定在右上角留白 8 像素
    if (m_toggleButton)
    {
        int titleBarHeight = (m_titleBar ? m_titleBar->height() : 48);
        int btnW = m_toggleButton->width();
        int x = this->width() - btnW - 8;
        int y = (titleBarHeight - m_toggleButton->height()) / 2;
        m_toggleButton->setGeometry(x, y, btnW, m_toggleButton->height());
    }
    // 更新内容区的位置与高度：使用窗口可用高度与最大高度的较小值，保证展开/折叠动画期间能正确显示子控件
    if (m_contentWidget)
    {
        int titleBarHeight = (m_titleBar ? m_titleBar->height() : 48);
        int availH = this->height() - titleBarHeight;
        if (availH < 0)
            availH = 0;
        int targetH = qMin(availH, m_contentWidget->maximumHeight());
        m_contentWidget->setGeometry(0, titleBarHeight, this->width(), targetH);
    }

    // 生成圆角遮罩：创建位图并绘制圆角矩形，用作窗口形状遮罩
    QSize s = this->size();
    QBitmap bitmap(s);
    bitmap.fill(Qt::color0);

    QPainter painter(&bitmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::color1);
    painter.setPen(Qt::NoPen);
    QRectF r(0.0, 0.0, s.width(), s.height());
    qreal radius = static_cast<qreal>(m_cornerRadius);
    painter.drawRoundedRect(r, radius, radius);
    painter.end();

    // 将生成的位图设置为窗口遮罩，从而实现圆角窗口效果
    this->setMask(QRegion(bitmap));
}

bool VoidCrawler::eventFilter(QObject* watched, QEvent* event)
{
    if (event == nullptr)
    {
        return QMainWindow::eventFilter(watched, event);
    }

    if ((watched == m_titleBar || watched == m_titleLabel) && event)
    {
        switch (event->type())
        {
        case QEvent::MouseButtonPress:
        {
            QMouseEvent* me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton)
            {
                // 开始拖动：记录鼠标相对于窗口左上角的偏移，用于后续移动
                m_dragging = true;
                m_dragPosition = me->globalPosition().toPoint() - this->frameGeometry().topLeft();
                return true; // 拦截事件
            }
            break;
        }
        case QEvent::MouseMove:
        {
            if (m_dragging)
            {
                QMouseEvent* me = static_cast<QMouseEvent*>(event);
                // 根据鼠标当前位置减去偏移量移动窗口
                this->move(me->globalPosition().toPoint() - m_dragPosition);
                return true; // 拦截事件
            }
            break;
        }
        case QEvent::MouseButtonRelease:
        {
            QMouseEvent* me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton)
            {
                // 结束拖动
                m_dragging = false;
                return true; // 拦截事件
            }
            break;
        }
        default:
            break;
        }
    }
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (watched->property("isItemButton").toBool())
        {
            if (mouseEvent->button() == Qt::RightButton)
            {
                on_button_right_clicked(
                    watched->property("itemID").toString(),
                    qobject_cast<QPushButton*>(watched));
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}