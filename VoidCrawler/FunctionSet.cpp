#include "FunctionSet.h"
#include <windows.h>
#include "Core.h"
#include <map>
#include <bitset>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <tlhelp32.h>
#include <cwctype>
#include <thread>
#include <chrono>
#include <fcntl.h>
#include <io.h>

void FunctionSet::scrollMouse(int delta)
{
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_WHEEL;
    input.mi.mouseData = delta;
    SendInput(1, &input, sizeof(INPUT));
}

void FunctionSet::clickMouse(int x, int y)
{
    SetCursorPos(x, y);
    INPUT down = { 0 };
    down.type = INPUT_MOUSE;
    down.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &down, sizeof(INPUT));
    INPUT up = { 0 };
    up.type = INPUT_MOUSE;
    up.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &up, sizeof(INPUT));
}

bool FunctionSet::modifyMicrosoftPinyinSwitchKey(const std::bitset<3>& bits)
{
    constexpr DWORD lookup_table[8] = {
       6,  // 000 -> 6
       5,  // 001 -> 5
       4,  // 010 -> 4
       7,  // 011 -> 7
       2,  // 100 -> 2
       1,  // 101 -> 1
       0,  // 110 -> 0
       3   // 111 -> 3
    };

    // 1. 定义目标注册表路径和键值
    LPCWSTR subKey = L"Software\\Microsoft\\InputMethod\\Settings\\CHS";
    LPCWSTR valueName = L"English Switch Key";

    // 2. 打开注册表项 (需要写入权限)
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, subKey, 0, KEY_WRITE, &hKey);

    if (result != ERROR_SUCCESS)
    {
        if (result == ERROR_FILE_NOT_FOUND)
        {
            VCCore::logger->error("The registry path does not exist. Please confirm that Microsoft Pinyin input method has been installed and started at least once.");
        }
        else
        {
            VCCore::logger->error(std::format("Unable to open registry key, error code: {}", result));
        }
        return false;
    }

    // 3. 设置新的键值数据
    DWORD newValue = lookup_table[bits.to_ulong()];

    result = RegSetValueExW(hKey, valueName, 0, REG_DWORD,
        (const BYTE*)&newValue, sizeof(newValue));

    // 4. 关闭注册表项
    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS)
    {
		VCCore::logger->error(std::format("Failed to modify Microsoft Pinyin switch key, error code: {}", result));
        return false;
    }

	VCCore::logger->info("Successfully modified Microsoft Pinyin switch key.");
    return true;
}

std::bitset<3> FunctionSet::getMicrosoftPinyinSwitchKey()
{
    // 逆向查找表，将注册表值映射回二进制
    constexpr std::bitset<3> reverse_lookup_table[8] = {
        std::bitset<3>("110"),  // 0 -> 110
        std::bitset<3>("101"),  // 1 -> 101
        std::bitset<3>("100"),  // 2 -> 100
        std::bitset<3>("111"),  // 3 -> 111
        std::bitset<3>("010"),  // 4 -> 010
        std::bitset<3>("001"),  // 5 -> 001
        std::bitset<3>("000"),  // 6 -> 000
        std::bitset<3>("011")   // 7 -> 011
    };

    // 1. 定义目标注册表路径和键值
    LPCWSTR subKey = L"Software\\Microsoft\\InputMethod\\Settings\\CHS";
    LPCWSTR valueName = L"English Switch Key";

    // 2. 打开注册表项 (需要读取权限)
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey);

    if (result != ERROR_SUCCESS)
    {
        if (result == ERROR_FILE_NOT_FOUND)
        {
            VCCore::logger->error("The registry path does not exist. Please confirm that Microsoft Pinyin input method has been installed and started at least once.");
        }
        else
        {
            VCCore::logger->error(std::format("Unable to open registry key, error code: {}", result));
        }
        // 返回一个默认值，或者可以根据需要抛出异常
        return std::bitset<3>(0);
    }

    // 3. 读取注册表值
    DWORD readValue = 0;
    DWORD dataSize = sizeof(readValue);
    DWORD type = REG_DWORD;

    result = RegQueryValueExW(hKey, valueName, nullptr, &type,
        (LPBYTE)&readValue, &dataSize);

    // 4. 关闭注册表项
    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS)
    {
        if (result == ERROR_FILE_NOT_FOUND)
        {
            VCCore::logger->error("The registry value does not exist. Using default value.");
        }
        else
        {
            VCCore::logger->error(std::format("Failed to read Microsoft Pinyin switch key, error code: {}", result));
        }
        // 返回默认值（通常为0，即二进制000）
        return std::bitset<3>(0);
    }

    // 5. 验证读取的值是否在有效范围内
    if (readValue >= 8)
    {
        VCCore::logger->warn(std::format("Unexpected registry value: {}, using default", readValue));
        return std::bitset<3>(0);
    }

    // 6. 使用逆向查找表将注册表值转换回二进制
    VCCore::logger->debug(std::format("Successfully read Microsoft Pinyin switch key: {} -> {}",
        readValue, reverse_lookup_table[readValue].to_string()));

    return reverse_lookup_table[readValue];
}

bool FunctionSet::FileExists(const std::string& path)
{
    return GetFileAttributesA(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

std::string FunctionSet::BuildExePath(const std::string& installPath)
{
    std::vector<std::string> exeNames = { "WeChat.exe", "Weixin.exe", "微信.exe" };

    for (const auto& exeName : exeNames)
    {
        std::string fullPath = installPath;
        if (!fullPath.empty() && fullPath.back() != '\\')
        {
            fullPath += '\\';
        }
        fullPath += exeName;

        if (FileExists(fullPath))
        {
            return fullPath;
        }
    }

    return "";
}

std::string FunctionSet::GetWeChatInstallPath()
{
    std::string installPath;
    HKEY hKey = nullptr;

    // 尝试 HKEY_CURRENT_USER (当前用户安装，优先级最高)
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Tencent\\Weixin", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        char buffer[MAX_PATH] = { 0 };
        DWORD bufferSize = sizeof(buffer);

        if (RegQueryValueExA(hKey, "InstallPath", nullptr, nullptr,
            reinterpret_cast<LPBYTE>(buffer), &bufferSize) == ERROR_SUCCESS)
        {
            installPath = buffer;
        }
        RegCloseKey(hKey);

        if (!installPath.empty())
        {
            std::string exePath = BuildExePath(installPath);
            if (!exePath.empty())
            {
                return exePath;
            }
        }
    }

    // 如果HKCU没有，尝试HKLM (所有用户安装)
    const char* hklmPaths[] =
    {
        "SOFTWARE\\Tencent\\WeChat",
        "SOFTWARE\\WOW6432Node\\Tencent\\WeChat",
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\WeChat",
        "SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\WeChat"
    };

    const char* valueNames[] = { "InstallPath", "InstallLocation", "UninstallString" };

    for (const auto& regPath : hklmPaths)
    {
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, regPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
        {
            for (const auto& valueName : valueNames)
            {
                char buffer[MAX_PATH] = { 0 };
                DWORD bufferSize = sizeof(buffer);

                if (RegQueryValueExA(hKey, valueName, nullptr, nullptr,
                    reinterpret_cast<LPBYTE>(buffer), &bufferSize) == ERROR_SUCCESS)
                {

                    std::string path = buffer;

                    // 如果是UninstallString，提取安装目录
                    if (std::string(valueName) == "UninstallString")
                    {
                        size_t pos = path.find("\\uninstall");
                        if (pos != std::string::npos)
                        {
                            installPath = path.substr(0, pos);
                        }
                    }
                    else
                    {
                        installPath = path;
                    }

                    if (!installPath.empty())
                    {
                        std::string exePath = BuildExePath(installPath);
                        if (!exePath.empty())
                        {
                            RegCloseKey(hKey);
                            return exePath;
                        }
                    }
                }
            }
            // if RegOpenKeyExA 内
            RegCloseKey(hKey);
        }
    }

    return "";
}

bool FunctionSet::IsProcessRunning(const std::wstring& processName)
{
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
		VCCore::logger->error(std::format("Failed to create process snapshot, error code: {}", GetLastError()));
        return false;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);
        return false;
    }

    do
    {
        if (_wcsicmp(pe32.szExeFile, processName.c_str()) == 0)
        {
            CloseHandle(hProcessSnap);
            return true;
        }
    } while (Process32NextW(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return false;
}

bool FunctionSet::SetWindowTransparency(HWND hwnd, BYTE alpha)
{
    // 获取当前窗口样式
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

    // 如果窗口没有 WS_EX_LAYERED 样式，则添加上去
    if (!(exStyle & WS_EX_LAYERED))
    {
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    }

    // 调用 API 设置透明度 (LWA_ALPHA 标志表示使用 alpha 值)
    return SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA) != 0;
}

HWND FunctionSet::FindWindowByTitle(const std::wstring& titleKeyword)
{
    struct FindData
    {
        std::wstring keyword;
        HWND result;
    } findData = { titleKeyword, NULL };

    // 枚举所有顶级窗口
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
        {
            FindData* data = reinterpret_cast<FindData*>(lParam);
            wchar_t windowTitle[256];

            // 获取窗口标题
            if (GetWindowTextW(hwnd, windowTitle, sizeof(windowTitle) / sizeof(wchar_t)))
            {
                std::wstring title(windowTitle);
                // 如果标题包含关键词，则找到目标窗口
                if (title.find(data->keyword) != std::wstring::npos)
                {
                    data->result = hwnd;
                    return FALSE; // 找到后停止枚举
                }
            }
            return TRUE; // 继续枚举
        },
        reinterpret_cast<LPARAM>(&findData));

    return findData.result;
}