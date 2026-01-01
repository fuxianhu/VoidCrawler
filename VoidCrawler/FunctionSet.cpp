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
    VCCore::logger->info(std::format("Successfully read Microsoft Pinyin switch key: {} -> {}",
        readValue, reverse_lookup_table[readValue].to_string()));

    return reverse_lookup_table[readValue];
}