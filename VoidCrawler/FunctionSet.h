#pragma once
#include <bitset>

namespace FunctionSet
{
	// 修改微软拼音输入法的英文切换快捷键，成功返回 true
	bool modifyMicrosoftPinyinSwitchKey(const std::bitset<3>& bits);
}