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
}