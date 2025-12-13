#include "FunctionSet.h"
#include <windows.h>
#include "Core.h"
#include <map>
#include <bitset>


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