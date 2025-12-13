#include <windows.h>
#include <iostream>

// 全局变量，用于存储钩子句柄和按键状态
HHOOK g_keyboardHook = NULL;
bool g_leftShiftPressed = false;
bool g_rightShiftPressed = false;

// 键盘钩子回调函数
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        // 检查是否为左Shift键
        if (p->vkCode == VK_LSHIFT) {
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                g_leftShiftPressed = true;
            } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                g_leftShiftPressed = false;
            }
        }
        // 检查是否为右Shift键
        else if (p->vkCode == VK_RSHIFT) {
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYUP) {
                g_rightShiftPressed = true;
            } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                g_rightShiftPressed = false;
            }
        }

        // 检测组合：当两个Shift都处于按下状态时触发
        if (g_leftShiftPressed && g_rightShiftPressed) {
            std::cout << "检测到组合键：左Shift + 右Shift" << std::endl;
            // 在这里执行你希望触发的操作
            // 注意：为了确保只触发一次，你可能需要添加额外的状态控制逻辑
        }
    }
    // 将事件传递给下一个钩子
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

int main() {
    // 设置全局键盘钩子
    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (g_keyboardHook == NULL) {
        std::cerr << "设置钩子失败！" << std::endl;
        return 1;
    }

    std::cout << "正在监听左Shift + 右Shift组合键..." << std::endl;
    std::cout << "按任意键退出程序。" << std::endl;

    // 保持程序运行的消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 程序退出前卸载钩子
    UnhookWindowsHookEx(g_keyboardHook);
    return 0;
}