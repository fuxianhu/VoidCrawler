#ifndef CRASH_HANDLER_H
#define CRASH_HANDLER_H

#include <windows.h>
#include <string>

class SimpleCrashHandler {
public:
    // 初始化崩溃处理器，必须在所有其他初始化之前调用
    static void Initialize(const wchar_t* dumpPath = nullptr);

    // 设置应用程序信息
    static void SetAppInfo(const wchar_t* appName, const wchar_t* appVersion);

private:
    // 私有构造函数，禁止实例化
    SimpleCrashHandler() = delete;

    // Windows异常处理函数
    static LONG WINAPI ExceptionHandler(PEXCEPTION_POINTERS exceptionInfo);

    // 生成崩溃报告
    static void GenerateCrashReport(PEXCEPTION_POINTERS exceptionInfo);

    // 获取异常描述
    static const wchar_t* GetExceptionDescription(DWORD exceptionCode);

    // 写入文件
    static bool WriteToFile(const wchar_t* filename, const wchar_t* content);

    // 静态成员
    static wchar_t s_appName[256];
    static wchar_t s_appVersion[64];
    static wchar_t s_dumpPath[MAX_PATH];
    static LPTOP_LEVEL_EXCEPTION_FILTER s_previousHandler;
};

#endif // CRASH_HANDLER_H