#pragma once
#include <windows.h>
#include <string>

enum class PrivilegeLevel
{
    Unknown = 0,
    StandardUser,      // 普通用户
    Administrator,     // Admin权限
    System,           // SYSTEM权限
    TrustedInstaller, // TrustedInstaller权限
    Other             // 其他权限
};

class PrivilegeChecker
{
public:

    // 获取当前进程权限级别
    static PrivilegeLevel GetCurrentPrivilegeLevel();

    // 获取权限级别的字符串描述
    static std::string GetPrivilegeLevelString(PrivilegeLevel level);
    static std::string GetCurrentPrivilegeLevelString();

    // 检查是否具有特定权限
    static bool IsRunningAsStandardUser();
    static bool IsRunningAsAdministrator();
    static bool IsRunningAsSystem();
    static bool IsRunningAsTrustedInstaller();

private:
    // 获取进程令牌
    static HANDLE GetProcessToken(HANDLE hProcess = nullptr);

    // 检查是否为管理员组
    static bool CheckAdministratorGroup();

    // 检查是否为SYSTEM账户
    static bool CheckSystemAccount(HANDLE hToken);

    // 检查是否为TrustedInstaller
    static bool CheckTrustedInstaller(HANDLE hToken);

    // 释放SID内存
    static void FreeSidMemory(PSID pSid);
};