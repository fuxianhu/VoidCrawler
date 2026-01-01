/*

项目主文件

E:\Qt\6.5.3\msvc2019_64\bin\lupdate .\VoidCrawler.vcxproj -ts .\translations\zh-CN.ts
E:\Qt\6.5.3\msvc2019_64\bin\lrelease .\translations\en-US.ts -qm .\translations\en-US.qm

链接器/系统/子系统 控制台或窗口

高考不方zhuangs
*/

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
static void parseArguments(const int &argc, char* argv[])
{
    // 遍历所有命令行参数
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "-version" || arg == "--version")
        {
            std::cout << "------------------------------" << std::endl;
            std::cout << " VoidCrawler Client" << std::endl;
            std::cout << " Version Number: " << VCCore::VoidCrawlerVersion.toStdString() << std::endl;
			std::cout << " Description: " << VCCore::VoidCrawlerVersion.DESCRIPTION.toStdString() << std::endl;
			std::cout << " Release Date: " << VCCore::VoidCrawlerVersion.BUILD_TIME.toString("yyyy/MM/dd").toStdString() << " UTC" << std::endl;
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

    // 记录开始时间
    QElapsedTimer timer;
    timer.start();

    // 立即显示启动画面
    QPixmap pixmap(VCCore::getPath("icon/startup.png"));
    QSplashScreen* splash = new QSplashScreen(pixmap);
    splash->show();
    QApplication::processEvents();

    VCCore::logger->info(std::format("Application started. Version: {}", VCCore::VoidCrawlerVersion.toStdString()));
    VCCore::logger->debug(VCCore::mainConfig.object().value("language").toString().toStdString());

    VoidCrawler window;
    window.changeLanguage(VCCore::mainConfig.object().value("language").toString());
    VCCore::logger->debug("init UI window...");
    window.initUI();

    qint64 elapsed = timer.elapsed();
    if (elapsed < STARTUP_SPLASH_DISPLAY_TIME)
    {
        QThread::msleep(STARTUP_SPLASH_DISPLAY_TIME - elapsed);
    }

    // 显示主窗口并关闭启动画面
    splash->finish(&window);
    delete splash;
    window.show(); // 请勿修改这 3 行的执行顺序！！！
    
    return QApplication::exec();
}