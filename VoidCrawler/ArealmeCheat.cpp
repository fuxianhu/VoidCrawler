#include "ArealmeCheat.h"
#include "FunctionSet.h"
#include "Core.h"

#include <windows.h>
#include <iostream>
#include <ctime>
#include <QMessageBox>
#include <QString>
#include <QApplication>


bool ArealmeCheat::cheatWarning()
{
    QMessageBox::StandardButton result = QMessageBox::warning(
        VCCore::mainWindow,
        "VoidCrawler Client Warning",
        R"(  ===== Warning 警告 =====
            1. 所有作弊行为可能会有 鼠标指针与键盘等输入设备控制、恶意 DLL 注入进程、内存修改行为，客户端上的此类功能均为未经授权的第三方程序（外挂、修改器、作弊器等）。
               对于游戏，这不仅破坏了其他玩家的体验和游戏的公平性，也让你自己的技术无法提升，剥夺了您通过正当方式获得成就的乐趣。
               使用作弊功能可能导致您的任何账号暂时或永久封禁、数据（如成绩、分数）无效、数据清零等风险。
            2. 同时，请自觉承担所有此类功能对你或你的财产、数据造成的影响！)",
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::NoButton
    );
    return result == QMessageBox::Ok;
}

int ArealmeCheat::testColorCheat()
{
    if (!cheatWarning())
    {
        return 1;
    }
    Sleep(2700);
    SetCursorPos(949, 636);
    FunctionSet::scrollMouse(10000);
    Sleep(300);
    FunctionSet::clickMouse(949, 636);
    Sleep(1000);
    time_t start_time = time(nullptr);
    while (difftime(time(nullptr), start_time) < 61)
    {
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 6; j++)
            {
                FunctionSet::clickMouse(670 + i * 100, 290 + j * 100);
                if ((GetAsyncKeyState(VK_SPACE) & 0x8000) != 0)
                {
                    return 2;
                }
            }
        }
    }
    return 0;
}

int ArealmeCheat::test1to50Cheat()
{
    if (!cheatWarning())
    {
        return 1;
    }
    Sleep(2700);
    SetCursorPos(954, 816);
    FunctionSet::scrollMouse(10000);
    Sleep(300);
    FunctionSet::clickMouse(954, 816);
    Sleep(1000);
    time_t start_time = time(nullptr);
    while (true)
    {
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                FunctionSet::clickMouse(790 + i * 80, 530 + j * 80);
                if ((GetAsyncKeyState(VK_SPACE) & 0x8000) != 0)
                {
                    return 2;
                }
            }
        }
    }
    return 0;
}