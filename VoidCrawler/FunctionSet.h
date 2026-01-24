#pragma once
#include <bitset>

namespace FunctionSet
{
	// 修改微软拼音输入法的英文切换快捷键，成功返回 true
	// // 提醒：bitset 下标是从右到左依次为 第0位、第1位、第2位
	// 1. Ctrl+Space 2. Shift 3. Ctrl
	bool modifyMicrosoftPinyinSwitchKey(const std::bitset<3>& bits);

	// 获取微软拼音输入法的英文切换快捷键，返回值为 3 位二进制表示，失败返回 000
	// 提醒：bitset 下标是从右到左依次为 第0位、第1位、第2位
	// 1. Ctrl+Space 2. Shift 3. Ctrl
	std::bitset<3> getMicrosoftPinyinSwitchKey();

	// 检查文件是否存在
	bool FileExists(const std::string& path);

	// 构建完整路径
	std::string BuildExePath(const std::string& installPath);

	// 获取微信安装路径，返回可执行文件完整路径，失败返回空字符串
	std::string GetWeChatInstallPath();
}