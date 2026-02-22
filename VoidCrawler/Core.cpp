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

int VCCore::startShellDetached(const QString& program, const QString& arguments)
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

    // ShellExecute 返回值大于32表示成功
    if ((INT_PTR)result > 32)
    {
        logger->debug(std::format("ShellExecute successful: {}", program.toStdString()));
        return 0;  // 成功返回0
    }
    else
    {
        // 获取错误码
        DWORD error = GetLastError();

        // 如果GetLastError返回0，则使用result作为错误码
        int errorCode = error;
        if (errorCode == 0)
        {
            errorCode = (INT_PTR)result;  // 使用ShellExecute返回的错误值
        }
        logger->error(std::format("ShellExecute failed for {}, Error code: {}", program.toStdString(), errorCode));

        return errorCode;  // 返回错误码
    }
}

int VCCore::runProgram(const QString& program, const QString& itemName, const QString& arguments)
{
    int mode = itemConfiguration.object().value(itemName).toObject().value("mode").toObject().value("value").toInt();
    int value;
    switch (mode)
    {
    case 0:
        value = system((program + arguments).toStdString().c_str());
        break;
    case 1:
        value = startShellDetached(program, arguments);
        break;
    default:
        value = -1;
        logger->error(std::format("VCCore::runProgram error: Read mode value error, options: {}, {}, {} Mode: {} Value: {}", program.toStdString(), itemName.toStdString(), arguments.toStdString(), mode, value));
        return value;
    }
    logger->info(std::format("VCCore::runProgram info: options: {}, {}, {} Mode: {} Value: {}", program.toStdString(), itemName.toStdString(), arguments.toStdString(), mode, value));
    return value;
}