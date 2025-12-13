#include "PrivilegeLevel.h"
#include <sddl.h>
#include <aclapi.h>
#include <iostream>

#pragma comment(lib, "advapi32.lib")

HANDLE PrivilegeChecker::GetProcessToken(HANDLE hProcess)
{
    HANDLE hToken = nullptr;

    if (hProcess == nullptr)
    {
        hProcess = GetCurrentProcess();
    }

    if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
    {
        std::cerr << "OpenProcessToken failed: " << GetLastError() << std::endl;
        return nullptr;
    }

    return hToken;
}

void PrivilegeChecker::FreeSidMemory(PSID pSid)
{
    if (pSid)
    {
        free(pSid);
    }
}

bool PrivilegeChecker::CheckAdministratorGroup() {

    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup = nullptr;
    BOOL b = AllocateAndInitializeSid(&NtAuthority, 2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &AdministratorsGroup);

    // 创建管理员组的SID
    if (b)
    {
        CheckTokenMembership(NULL, AdministratorsGroup, &b);
        FreeSid(AdministratorsGroup);
    }
    return b;
}

bool PrivilegeChecker::CheckSystemAccount(HANDLE hToken)
{
    if (!hToken) return false;
    
    DWORD dwTokenInfoLength = 0;
    PTOKEN_USER pTokenUser = nullptr;
    BOOL isSystem = FALSE;

    // 第一次调用获取所需缓冲区大小
    GetTokenInformation(hToken, TokenUser, nullptr, 0, &dwTokenInfoLength);

    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    {
        return false;
    }

    // 分配缓冲区
    pTokenUser = (PTOKEN_USER)malloc(dwTokenInfoLength);
    if (!pTokenUser)
    {
        return false;
    }

    // 获取令牌用户信息
    if (GetTokenInformation(hToken, TokenUser, pTokenUser, dwTokenInfoLength, &dwTokenInfoLength))
    {
        // 检查是否为SYSTEM SID (S-1-5-18)
        SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
        PSID systemSid = nullptr;
        if (AllocateAndInitializeSid(&ntAuthority, 1, SECURITY_LOCAL_SYSTEM_RID,
            0, 0, 0, 0, 0, 0, 0, &systemSid)) {
            isSystem = EqualSid(pTokenUser->User.Sid, systemSid);
            FreeSid(systemSid);
        }
    }

    free(pTokenUser);
    return isSystem;
}

bool PrivilegeChecker::CheckTrustedInstaller(HANDLE hToken)
{
    if (!hToken) return false;
    
    DWORD dwTokenInfoLength = 0;
    PTOKEN_GROUPS pTokenGroups = nullptr;
    BOOL isTrustedInstaller = FALSE;

    // 第一次调用获取所需缓冲区大小
    GetTokenInformation(hToken, TokenGroups, nullptr, 0, &dwTokenInfoLength);

    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    {
        return false;
    }

    // 分配缓冲区
    pTokenGroups = (PTOKEN_GROUPS)malloc(dwTokenInfoLength);
    if (!pTokenGroups)
    {
        return false;
    }

    // 获取令牌组信息
    if (GetTokenInformation(hToken, TokenGroups, pTokenGroups, dwTokenInfoLength, &dwTokenInfoLength))
    {
        // TrustedInstaller的SID: S-1-5-80-956008885-3418522649-1831038044-1853292631-2271478464
        const wchar_t* trustedInstallerSid = L"S-1-5-80-956008885-3418522649-1831038044-1853292631-2271478464";
        
        PSID trustedInstallerSidPtr = nullptr;
        if (ConvertStringSidToSidW(trustedInstallerSid, &trustedInstallerSidPtr))
        {
            // 遍历所有组SID，检查是否包含TrustedInstaller组
            for (DWORD i = 0; i < pTokenGroups->GroupCount; i++)
            {
                if (EqualSid(pTokenGroups->Groups[i].Sid, trustedInstallerSidPtr))
                {
                    isTrustedInstaller = TRUE;
                    break;
                }
            }
            LocalFree(trustedInstallerSidPtr);
        }
    }

    free(pTokenGroups);
    return isTrustedInstaller;
}

PrivilegeLevel PrivilegeChecker::GetCurrentPrivilegeLevel()
{
    HANDLE hToken = GetProcessToken();
    if (!hToken)
    {
        return PrivilegeLevel::Unknown;
    }

    PrivilegeLevel level = PrivilegeLevel::Other;

    // 检查TrustedInstaller权限（最高权限）
    if (CheckTrustedInstaller(hToken))
    {
        level = PrivilegeLevel::TrustedInstaller;
    }
    // 检查SYSTEM权限
    else if (CheckSystemAccount(hToken))
    {
        level = PrivilegeLevel::System;
    }
    // 检查管理员权限
    else if (CheckAdministratorGroup())
    {
        level = PrivilegeLevel::Administrator;
    }
    // 默认为普通用户
    else
    {
        level = PrivilegeLevel::StandardUser;
    }

    CloseHandle(hToken);
    return level;
}

std::string PrivilegeChecker::GetPrivilegeLevelString(PrivilegeLevel level)
{
    switch (level)
    {
    case PrivilegeLevel::StandardUser:
        return "Standard User";
    case PrivilegeLevel::Administrator:
        return "Administrator";
    case PrivilegeLevel::System:
        return "SYSTEM";
    case PrivilegeLevel::TrustedInstaller:
        return "TrustedInstaller";
    case PrivilegeLevel::Other:
        return "Other";
    default:
        return "Unknown";
    }
}

std::string PrivilegeChecker::GetCurrentPrivilegeLevelString()
{
    return GetPrivilegeLevelString(GetCurrentPrivilegeLevel());
}

bool PrivilegeChecker::IsRunningAsStandardUser()
{
    return GetCurrentPrivilegeLevel() == PrivilegeLevel::StandardUser;
}

bool PrivilegeChecker::IsRunningAsAdministrator()
{
    return GetCurrentPrivilegeLevel() == PrivilegeLevel::Administrator;
}

bool PrivilegeChecker::IsRunningAsSystem()
{
    return GetCurrentPrivilegeLevel() == PrivilegeLevel::System;
}

bool PrivilegeChecker::IsRunningAsTrustedInstaller()
{
    return GetCurrentPrivilegeLevel() == PrivilegeLevel::TrustedInstaller;
}