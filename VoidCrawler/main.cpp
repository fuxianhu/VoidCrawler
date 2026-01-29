/*

项目主文件

E:\Qt\6.5.3\msvc2019_64\bin\lupdate .\VoidCrawler.vcxproj -ts .\translations\zh-CN.ts
E:\Qt\6.5.3\msvc2019_64\bin\lrelease .\translations\en-US.ts -qm .\translations\en-US.qm

链接器/系统/子系统 控制台或窗口

His Theme
https://www.bilibili.com/video/BV1Y4AGe9EVo/
https://www.bilibili.com/video/BV1nkmQBaEVM/

窗口强制置顶：（第一步在程序初始化前从winlogon.exe偷UIAccess令牌）
https://www.bilibili.com/video/BV1HCwwegEVp

tap hold flick drag
*/

#include "crash_handler.h"

class EarlyInit
{
public:
    EarlyInit()
    {
        SimpleCrashHandler::SetAppInfo(L"MyQtApp", L"1.0.0");
        SimpleCrashHandler::Initialize(L".\\crash_reports");
    }
};

static EarlyInit g_earlyInit;

#include "VoidCrawler.h"
#include "LoggerManager.h"
#include "configOperate.h"
#include "Core.h"

#include <unordered_map>
#include <memory>
#include <cstdlib>
#include <iostream>
#include <format>
#include <windows.h>

#include <QDir>
#include <QMessageBox>
#include <QtWidgets/QApplication>
#include <QDebug>
#include <QTranslator>
#include <QObject>
#include <QSplashScreen>
#include <QTimer>
#include <QThread>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>


// 解析命令行参数
static void parseArguments(const int& argc, char* argv[])
{
    // 遍历所有命令行参数
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "-version" || arg == "--version")
        {
            // 确保控制台输出可用
            if (!GetConsoleWindow()) {
                AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole();
                FILE* fp;
                freopen_s(&fp, "CONOUT$", "w", stdout);
                freopen_s(&fp, "CONOUT$", "w", stderr);
                std::ios::sync_with_stdio();
            }
            std::cout << std::endl << std::endl;
            std::cout << "------------------------------" << std::endl;
            std::cout << " VoidCrawler Client" << std::endl;
            std::cout << " Version Number: " << VCCore::VoidCrawlerVersion.toStdString() << std::endl;
            std::cout << " Description: " << VCCore::VoidCrawlerVersion.DESCRIPTION.toStdString() << std::endl;
            std::cout << " Release Date: " << VCCore::VoidCrawlerVersion.BUILD_TIME.toString("yyyy/MM/dd").toStdString() << ' ' << CLIENT_BUILD_DATETIME_TIMEZONE << std::endl;
            std::cout << "------------------------------" << std::endl;
            std::exit(0); // 输出版本后直接退出
        }
    }
}

int main(int argc, char* argv[])
{
    parseArguments(argc, argv);

    QLocale::setDefault(QLocale::system());
    QApplication app(argc, argv);

    QElapsedTimer timer;
    timer.start();

    QPixmap* pixmap = new QPixmap(VCCore::getPath("icon/startup.png"));
    QSplashScreen* splash = new QSplashScreen(*pixmap);
    splash->show();
    QApplication::processEvents();

    VCCore::logger->info(std::format("Application started. Version: {}", VCCore::VoidCrawlerVersion.toStdString()));
    VCCore::logger->debug(VCCore::mainConfig.object().value("language").toString().toStdString());

    VoidCrawler window;
    window.changeLanguage(VCCore::mainConfig.object().value("language").toString());
    VCCore::logger->debug("init UI window...");
    window.initUI();

    {
        qint64 elapsed = timer.elapsed();
        if (elapsed < STARTUP_SPLASH_DISPLAY_TIME)
        {
            QThread::msleep(STARTUP_SPLASH_DISPLAY_TIME - elapsed);
        }
    }

    window.show();
    splash->finish(&window);
    splash->hide();
    splash->close();
    delete splash;
    delete pixmap;
    VCCore::logger->debug(std::format("Initialization completed. Duration: {} milliseconds. ", timer.elapsed()));

    return QApplication::exec();
}