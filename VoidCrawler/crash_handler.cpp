#include "crash_handler.h"
#include "ClientInfo.h"

#include <dbghelp.h>
#include <tlhelp32.h>
#include <VersionHelpers.h>  // Windows版本检查的新API
#include <ctime>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

#pragma comment(lib, "dbghelp.lib")

// 初始化静态成员
wchar_t SimpleCrashHandler::s_appName[256] = L_APP_NAME;
wchar_t SimpleCrashHandler::s_appVersion[64] = L_CLIENT_VERSION_NUMBER;
wchar_t SimpleCrashHandler::s_dumpPath[MAX_PATH] = L_CRASH_REPORTS_PATH;
LPTOP_LEVEL_EXCEPTION_FILTER SimpleCrashHandler::s_previousHandler = nullptr;

void SimpleCrashHandler::Initialize(const wchar_t* dumpPath) {
    // 设置dump路径
    if (dumpPath && wcslen(dumpPath) > 0) {
        wcscpy_s(s_dumpPath, MAX_PATH, dumpPath);
    }

    // 创建目录
    CreateDirectoryW(s_dumpPath, nullptr);

    // 设置异常处理器
    s_previousHandler = SetUnhandledExceptionFilter(ExceptionHandler);
}

void SimpleCrashHandler::SetAppInfo(const wchar_t* appName, const wchar_t* appVersion) {
    if (appName) {
        wcscpy_s(s_appName, 256, appName);
    }
    if (appVersion) {
        wcscpy_s(s_appVersion, 64, appVersion);
    }
}

LONG WINAPI SimpleCrashHandler::ExceptionHandler(PEXCEPTION_POINTERS exceptionInfo) {
    // 生成崩溃报告
    GenerateCrashReport(exceptionInfo);

    // 如果之前有异常处理器，调用它
    if (s_previousHandler) {
        return s_previousHandler(exceptionInfo);
    }

    // 否则终止程序
    return EXCEPTION_EXECUTE_HANDLER;
}

// 获取Windows版本信息的替代方法
std::wstring GetWindowsVersion() {
    std::wstring version = L"Windows ";

    // 使用VersionHelpers.h中的宏
    if (IsWindows10OrGreater()) {
        // 尝试获取具体版本号
        HMODULE hModule = LoadLibraryW(L"ntdll.dll");
        if (hModule) {
            typedef LONG(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
            RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hModule, "RtlGetVersion");

            if (RtlGetVersion) {
                RTL_OSVERSIONINFOW osInfo = { 0 };
                osInfo.dwOSVersionInfoSize = sizeof(osInfo);

                if (RtlGetVersion(&osInfo) == 0) {
                    version += L"10 or later";
                    version += L" (Build " + std::to_wstring(osInfo.dwBuildNumber) + L")";
                }
            }

            FreeLibrary(hModule);
        }
        else {
            version += L"10 or later";
        }
    }
    else if (IsWindows8Point1OrGreater()) {
        version += L"8.1 or later";
    }
    else if (IsWindows8OrGreater()) {
        version += L"8 or later";
    }
    else if (IsWindows7OrGreater()) {
        version += L"7 or later";
    }
    else if (IsWindowsVistaOrGreater()) {
        version += L"Vista or later";
    }
    else if (IsWindowsXPOrGreater()) {
        version += L"XP or later";
    }
    else {
        version += L"Unknown version";
    }

    return version;
}

// 添加这个函数来获取堆栈跟踪
std::wstring GetStackTrace(PEXCEPTION_POINTERS exceptionInfo) {
    std::wstringstream stackTrace;
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();

    // 初始化符号处理器 - 必须在函数内部初始化
    if (!SymInitialize(process, nullptr, TRUE)) {
        stackTrace << L"Failed to initialize symbol handler. Error: "
            << GetLastError() << L"\n";
        return stackTrace.str();
    }

    // 设置符号选项
    SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);

    // 设置搜索路径（包含当前目录）
    wchar_t searchPath[MAX_PATH];
    if (GetModuleFileNameW(nullptr, searchPath, MAX_PATH)) {
        // 移除文件名部分，只保留目录
        wchar_t* lastSlash = wcsrchr(searchPath, L'\\');
        if (lastSlash) {
            *lastSlash = L'\0';
            SymSetSearchPathW(process, searchPath);
        }
    }

    // 准备堆栈帧
    STACKFRAME64 stackFrame = {};
    CONTEXT context = {};

    if (exceptionInfo) {
        context = *exceptionInfo->ContextRecord;
    }
    else {
        // 如果没有异常信息，捕获当前上下文
        RtlCaptureContext(&context);
    }

    // 设置初始堆栈帧
#ifdef _WIN64
    stackFrame.AddrPC.Offset = context.Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Rbp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
    DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
#else
    stackFrame.AddrPC.Offset = context.Eip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Ebp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.Esp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
    DWORD machineType = IMAGE_FILE_MACHINE_I386;
#endif

    int frameCount = 0;
    const int maxFrames = 50;

    while (StackWalk64(machineType, process, thread, &stackFrame, &context,
        nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr)) {

        if (stackFrame.AddrPC.Offset == 0 || frameCount >= maxFrames) {
            break;
        }

        stackTrace << L"[" << frameCount << "] ";

        // 获取模块信息
        HMODULE hModule = nullptr;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCTSTR)stackFrame.AddrPC.Offset, &hModule);

        if (hModule) {
            wchar_t moduleName[MAX_PATH];
            if (GetModuleFileNameW(hModule, moduleName, MAX_PATH)) {
                wchar_t* baseName = wcsrchr(moduleName, L'\\');
                if (baseName) baseName++;
                else baseName = moduleName;

                stackTrace << baseName << L"!";
            }
        }

        // 尝试获取符号信息
        DWORD64 displacement = 0;
        char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)] = {};
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)symbolBuffer;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;

        if (SymFromAddr(process, stackFrame.AddrPC.Offset, &displacement, symbol)) {
            // 转换为宽字符串
            char narrowName[MAX_SYM_NAME];
            strncpy_s(narrowName, symbol->Name, _TRUNCATE);

            wchar_t wideName[MAX_SYM_NAME];
            size_t converted = 0;
            mbstowcs_s(&converted, wideName, narrowName, MAX_SYM_NAME);

            stackTrace << wideName;
            if (displacement > 0) {
                stackTrace << L"+0x" << std::hex << displacement << std::dec;
            }
        }
        else {
            // 没有符号信息，只显示地址
            stackTrace << L"0x" << std::hex << stackFrame.AddrPC.Offset << std::dec;
        }

        // 尝试获取源代码行信息
        IMAGEHLP_LINE64 line = {};
        line.SizeOfStruct = sizeof(line);
        DWORD lineDisplacement = 0;

        if (SymGetLineFromAddr64(process, stackFrame.AddrPC.Offset, &lineDisplacement, &line)) {
            stackTrace << L" [" << line.FileName << L":" << line.LineNumber << L"]";
        }

        stackTrace << L"\n";
        frameCount++;
    }

    if (frameCount == 0) {
        stackTrace << L"No stack frames could be walked.\n";
        stackTrace << L"Possible reasons:\n";
        stackTrace << L"1. Stack is corrupted\n";
        stackTrace << L"2. Context information is invalid\n";
        stackTrace << L"3. PDB files not found in executable directory\n";
    }

    // 清理符号处理器
    SymCleanup(process);

    return stackTrace.str();
}

void SimpleCrashHandler::GenerateCrashReport(PEXCEPTION_POINTERS exceptionInfo) {
    // 获取当前时间
    time_t now = time(nullptr);
    tm localTime;
    localtime_s(&localTime, &now);

    // 生成文件名
    wchar_t filename[MAX_PATH];
    swprintf_s(filename, MAX_PATH, L"%s\\crash_%04d%02d%02d_%02d%02d%02d.txt",
        s_dumpPath,
        localTime.tm_year + 1900,
        localTime.tm_mon + 1,
        localTime.tm_mday,
        localTime.tm_hour,
        localTime.tm_min,
        localTime.tm_sec);

    std::wstringstream report;

    // 报告头部
    report << L"=== Application Crash Report ===\n\n";
    report << L"Application: " << s_appName << L"\n";
    report << L"Version: " << s_appVersion << L"\n";

    // 时间
    wchar_t timeStr[64];
    wcsftime(timeStr, 64, L"%Y-%m-%d %H:%M:%S", &localTime);
    report << L"Crash Time: " << timeStr << L"\n\n";

    // 异常信息
    if (exceptionInfo && exceptionInfo->ExceptionRecord) {
        DWORD exceptionCode = exceptionInfo->ExceptionRecord->ExceptionCode;
        report << L"Exception Code: 0x" << std::hex << exceptionCode << std::dec << L"\n";
        report << L"Exception: " << GetExceptionDescription(exceptionCode) << L"\n";
        report << L"Exception Address: 0x" << std::hex
            << (DWORD_PTR)exceptionInfo->ExceptionRecord->ExceptionAddress << std::dec << L"\n\n";

        // 如果是访问违规，显示更多信息
        if (exceptionCode == EXCEPTION_ACCESS_VIOLATION) {
            DWORD_PTR accessAddress = exceptionInfo->ExceptionRecord->ExceptionInformation[1];
            DWORD accessType = exceptionInfo->ExceptionRecord->ExceptionInformation[0];

            report << L"Access Type: " << (accessType == 0 ? L"Read" :
                accessType == 1 ? L"Write" :
                accessType == 8 ? L"DEP Violation" : L"Unknown") << L"\n";
            report << L"Access Address: 0x" << std::hex << accessAddress << std::dec << L"\n";
        }
    }

    // 系统信息
    report << L"\n=== System Information ===\n";

    // Windows版本
    std::wstring winVersion = GetWindowsVersion();
    report << L"OS: " << winVersion << L"\n";

    // 系统信息
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    report << L"CPU Architecture: ";
    switch (sysInfo.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64: report << L"x64"; break;
    case PROCESSOR_ARCHITECTURE_ARM: report << L"ARM"; break;
    case PROCESSOR_ARCHITECTURE_ARM64: report << L"ARM64"; break;
    case PROCESSOR_ARCHITECTURE_IA64: report << L"Itanium"; break;
    case PROCESSOR_ARCHITECTURE_INTEL: report << L"x86"; break;
    default: report << L"Unknown"; break;
    }
    report << L"\n";

    report << L"CPU Cores: " << sysInfo.dwNumberOfProcessors << L"\n";
    report << L"Page Size: " << sysInfo.dwPageSize << L" bytes\n";

    // 内存信息
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);
    GlobalMemoryStatusEx(&memoryStatus);

    report << L"Total RAM: " << (memoryStatus.ullTotalPhys / (1024 * 1024)) << L" MB\n";
    report << L"Available RAM: " << (memoryStatus.ullAvailPhys / (1024 * 1024)) << L" MB\n";

    // 进程信息
    report << L"\n=== Process Information ===\n";
    report << L"Process ID: " << GetCurrentProcessId() << L"\n";
    report << L"Thread ID: " << GetCurrentThreadId() << L"\n";

    // 堆栈跟踪 - 这是修复的关键部分！
    report << L"\n=== Stack Trace ===\n";
    report << GetStackTrace(exceptionInfo);

    // 加载的模块
    report << L"\n=== Loaded Modules (Top ";
    report << LOADED_MODULES_REPORT_MAXIMUM_LSTRING;
    report << L") == = \n";

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (snapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32W module;
        module.dwSize = sizeof(module);

        int moduleCount = 0;
        if (Module32FirstW(snapshot, &module)) {
            do {
                if (moduleCount++ >= LOADED_MODULES_REPORT_MAXIMUM) {
                    report << L"... and " << (moduleCount - LOADED_MODULES_REPORT_MAXIMUM) << L" more modules\n";
                    break;
                }

                report << L"  " << module.szModule
                    << L" (0x" << std::hex << (DWORD_PTR)module.modBaseAddr << std::dec
                    << L", " << module.modBaseSize << L" bytes)\n";
            } while (Module32NextW(snapshot, &module));
        }

        CloseHandle(snapshot);
    }

    // 写入文件
    std::wstring reportStr = report.str();
    WriteToFile(filename, reportStr.c_str());

    // 显示消息框
    wchar_t message[512];
    swprintf_s(message, 512,
        L"%s has crashed.\n\n"
        L"A detailed crash report has been saved to:\n%s\n\n"
        L"Please send this report to the developer.",
        s_appName, filename);

    MessageBoxW(nullptr, message, L"Application Crash", MB_OK | MB_ICONERROR | MB_TOPMOST);
}

const wchar_t* SimpleCrashHandler::GetExceptionDescription(DWORD exceptionCode) {
    switch (exceptionCode) {
    case EXCEPTION_ACCESS_VIOLATION:         return L"Access violation";
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return L"Array bounds exceeded";
    case EXCEPTION_BREAKPOINT:               return L"Breakpoint";
    case EXCEPTION_DATATYPE_MISALIGNMENT:    return L"Data type misalignment";
    case EXCEPTION_FLT_DENORMAL_OPERAND:     return L"Floating-point denormal operand";
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return L"Floating-point divide by zero";
    case EXCEPTION_FLT_INEXACT_RESULT:       return L"Floating-point inexact result";
    case EXCEPTION_FLT_INVALID_OPERATION:    return L"Floating-point invalid operation";
    case EXCEPTION_FLT_OVERFLOW:             return L"Floating-point overflow";
    case EXCEPTION_FLT_STACK_CHECK:          return L"Floating-point stack check";
    case EXCEPTION_FLT_UNDERFLOW:            return L"Floating-point underflow";
    case EXCEPTION_ILLEGAL_INSTRUCTION:      return L"Illegal instruction";
    case EXCEPTION_IN_PAGE_ERROR:            return L"In-page error";
    case EXCEPTION_INT_DIVIDE_BY_ZERO:       return L"Integer divide by zero";
    case EXCEPTION_INT_OVERFLOW:             return L"Integer overflow";
    case EXCEPTION_INVALID_DISPOSITION:      return L"Invalid disposition";
    case EXCEPTION_NONCONTINUABLE_EXCEPTION: return L"Noncontinuable exception";
    case EXCEPTION_PRIV_INSTRUCTION:         return L"Privileged instruction";
    case EXCEPTION_SINGLE_STEP:              return L"Single step";
    case EXCEPTION_STACK_OVERFLOW:           return L"Stack overflow";
    default:                                 return L"Unknown exception";
    }
}

bool SimpleCrashHandler::WriteToFile(const wchar_t* filename, const wchar_t* content) {
    std::wofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
        return true;
    }
    return false;
}