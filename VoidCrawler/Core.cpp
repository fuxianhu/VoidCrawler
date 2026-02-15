#include "Core.h"


void VCCore::hideAllTopLevelWindows()
{
    // 所有顶级窗口的列表
    QWidgetList topLevelWidgets = QApplication::topLevelWidgets();
    // 遍历列表并隐藏每个窗口
    for (QWidget* widget : topLevelWidgets)
    {
        if (widget && widget->isWindow())
        {
            widget->hide();
        }
    }
}

void VCCore::showAllTopLevelWindows()
{
    // 所有顶级窗口的列表
    QWidgetList topLevelWidgets = QApplication::topLevelWidgets();
    // 遍历列表并隐藏每个窗口
    for (QWidget* widget : topLevelWidgets)
    {
        if (widget && widget->isWindow())
        {
            widget->show();
        }
    }
}

void VCCore::startShellDetached(const QString& program, const QString& arguments)
{
    HINSTANCE result = ShellExecuteW
    (
        nullptr,                          // 父窗口句柄
        L"open",                          // 操作（打开）
        program.toStdWString().c_str(),   // 文件/程序
        arguments.toStdWString().c_str(), // 参数
        nullptr,                          // 工作目录
        SW_SHOWNORMAL                     // 显示方式
    );

    if ((INT_PTR)result <= 32)
    {
        DWORD error = GetLastError();
        logger->error(std::format("ShellExecute failed for {}, Error: {}",
            program.toStdString(), error));
    }
    else
    {
        logger->debug(std::format("ShellExecute successful: {}", program.toStdString()));
    }
}

void VCCore::runProgram(const QString& program, const QString& itemName, const QString& arguments = nullptr)
{
    int mode = itemConfiguration.object().value(itemName).toObject().value("mode").toInt();
    switch (mode)
    {
    case 0:
        system((program + arguments).toStdString().c_str());
        break;
    case 1:
        startShellDetached(program, arguments);
        break;
    default:
        logger->error(std::format("VCCore::runProgram Error: Read mode value error, options: {}, {}, {}  Value: {}", program, itemName, arguments, mode));
    }
    
}