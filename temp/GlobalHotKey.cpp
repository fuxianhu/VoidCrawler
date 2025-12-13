#include "GlobalHotKey.h"
#include <iostream>

// 初始化静态成员
GlobalHotKeyDetector* GlobalHotKeyDetector::s_instance = nullptr;

// 构造函数
GlobalHotKeyDetector::GlobalHotKeyDetector(HotkeyMode mode) 
    : m_virtualKey('S')  // 默认使用 'S' 键
    , m_modifiers(MOD_CONTROL | MOD_WIN | MOD_SHIFT)
    , m_mode(mode)
    , m_isRunning(false)
    , m_hotkeyRegistered(false)
    , m_hWnd(nullptr)
    , m_hotkeyId(1)
    , m_keyboardHook(nullptr)
    , m_callback(nullptr)
    , m_ctrlPressed(false)
    , m_winPressed(false)
    , m_shiftPressed(false)
    , m_targetKeyPressed(false) {
}

// 析构函数
GlobalHotKeyDetector::~GlobalHotKeyDetector() {
    StopDetection();
}

// 设置热键
bool GlobalHotKeyDetector::SetHotkey(UINT vk, HotkeyMode mode) {
    if (m_isRunning) {
        std::cerr << "错误：检测器正在运行，请先停止再设置热键" << std::endl;
        return false;
    }
    
    m_mode = mode;
    
    if (vk != 0) {
        m_virtualKey = vk;
    }
    
    return true;
}

// 启动热键检测
bool GlobalHotKeyDetector::StartDetection(HWND hWnd, HotkeyCallback callback) {
    if (m_isRunning) {
        std::cerr << "警告：检测器已经在运行" << std::endl;
        return true;
    }
    
    m_hWnd = hWnd;
    m_callback = callback;
    
    if (m_mode == HotkeyMode::RegisterHotKey) {
        // RegisterHotKey 模式
        if (!RegisterSystemHotkey()) {
            std::cerr << "错误：注册系统热键失败" << std::endl;
            return false;
        }
        m_hotkeyRegistered = true;
        std::cout << "RegisterHotKey模式：已注册 Ctrl+Win+Shift+" 
                  << static_cast<char>(m_virtualKey) << " 热键" << std::endl;
    } 
    else {
        // LowLevelHook 模式
        if (!InstallHook()) {
            std::cerr << "错误：安装键盘钩子失败" << std::endl;
            return false;
        }
        std::cout << "LowLevelHook模式：开始监听 Ctrl+Win+Shift 组合键" << std::endl;
    }
    
    m_isRunning = true;
    return true;
}

// 停止热键检测
void GlobalHotKeyDetector::StopDetection() {
    if (!m_isRunning) return;
    
    if (m_mode == HotkeyMode::RegisterHotKey) {
        UnregisterSystemHotkey();
        std::cout << "RegisterHotKey模式：已注销热键" << std::endl;
    } 
    else {
        UninstallHook();
        std::cout << "LowLevelHook模式：已卸载键盘钩子" << std::endl;
    }
    
    m_isRunning = false;
    m_hotkeyRegistered = false;
}

// 处理窗口消息
void GlobalHotKeyDetector::ProcessWindowMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    if (m_mode != HotkeyMode::RegisterHotKey || !m_isRunning) return;
    
    if (message == WM_HOTKEY) {
        if (static_cast<int>(wParam) == m_hotkeyId) {
            std::cout << "热键触发: Ctrl+Win+Shift+" 
                      << static_cast<char>(m_virtualKey) << std::endl;
            
            if (m_callback) {
                m_callback();
            }
        }
    }
}

// 检查热键是否可用
bool GlobalHotKeyDetector::IsHotkeyAvailable(UINT modifiers, UINT vk) {
    // 尝试临时注册热键来检查是否可用
    HWND tempHwnd = GetConsoleWindow();
    int tempId = 9999;  // 临时ID
    
    bool available = RegisterHotKey(tempHwnd, tempId, modifiers, vk);
    
    if (available) {
        UnregisterHotKey(tempHwnd, tempId);
    }
    
    return available;
}

// ========== 私有方法实现 ==========

// 注册系统热键
bool GlobalHotKeyDetector::RegisterSystemHotkey() {
    if (!m_hWnd) {
        // 如果没有传入窗口句柄，使用控制台窗口
        m_hWnd = GetConsoleWindow();
    }
    
    if (!m_hWnd) {
        std::cerr << "错误：没有有效的窗口句柄用于接收热键消息" << std::endl;
        return false;
    }
    
    // 检查热键是否可用
    if (!IsHotkeyAvailable(m_modifiers, m_virtualKey)) {
        std::cerr << "错误：热键已被占用，请尝试其他按键组合" << std::endl;
        return false;
    }
    
    // 注册热键
    if (!RegisterHotKey(m_hWnd, m_hotkeyId, m_modifiers, m_virtualKey)) {
        DWORD error = GetLastError();
        std::cerr << "错误：RegisterHotKey失败，错误代码: " << error << std::endl;
        return false;
    }
    
    return true;
}

// 注销系统热键
void GlobalHotKeyDetector::UnregisterSystemHotkey() {
    if (m_hotkeyRegistered && m_hWnd) {
        UnregisterHotKey(m_hWnd, m_hotkeyId);
    }
}

// 安装钩子
bool GlobalHotKeyDetector::InstallHook() {
    if (m_keyboardHook) {
        return true;  // 钩子已存在
    }
    
    // 设置静态实例指针
    s_instance = this;
    
    // 安装低级键盘钩子
    m_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(nullptr), 0);
    
    if (!m_keyboardHook) {
        DWORD error = GetLastError();
        std::cerr << "错误：SetWindowsHookEx失败，错误代码: " << error << std::endl;
        s_instance = nullptr;
        return false;
    }
    
    return true;
}

// 卸载钩子
void GlobalHotKeyDetector::UninstallHook() {
    if (m_keyboardHook) {
        UnhookWindowsHookEx(m_keyboardHook);
        m_keyboardHook = nullptr;
        s_instance = nullptr;
    }
}

// 钩子回调函数 (静态)
LRESULT CALLBACK GlobalHotKeyDetector::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
    
    if (!s_instance) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
    
    KBDLLHOOKSTRUCT* pKeyInfo = (KBDLLHOOKSTRUCT*)lParam;
    UINT vkCode = pKeyInfo->vkCode;
    bool keyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
    
    // 处理按键
    s_instance->ProcessKeyCombination(vkCode, keyDown);
    
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// 处理按键组合
void GlobalHotKeyDetector::ProcessKeyCombination(UINT vkCode, bool keyDown) {
    // 更新修饰键状态
    if (vkCode == VK_CONTROL || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL) {
        m_ctrlPressed = keyDown;
    }
    else if (vkCode == VK_LWIN || vkCode == VK_RWIN) {
        m_winPressed = keyDown;
    }
    else if (vkCode == VK_SHIFT || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT) {
        m_shiftPressed = keyDown;
    }
    else if (vkCode == m_virtualKey) {
        m_targetKeyPressed = keyDown;
        
        // 检查是否按下目标键且三个修饰键都处于按下状态
        if (keyDown && m_ctrlPressed && m_winPressed && m_shiftPressed) {
            std::cout << "钩子检测到: Ctrl+Win+Shift+" 
                      << static_cast<char>(m_virtualKey) << std::endl;
            
            if (m_callback) {
                m_callback();
            }
            
            // 可以在这里阻止按键继续传递
            // return; // 注意：实际应在LowLevelKeyboardProc中返回非零值
        }
    }
}

// 检查三个修饰键是否同时按下
bool GlobalHotKeyDetector::CheckModifierKeys() {
    // 使用GetAsyncKeyState检查按键状态
    bool ctrlDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    bool winDown = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 || 
                   (GetAsyncKeyState(VK_RWIN) & 0x8000) != 0;
    bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    
    return ctrlDown && winDown && shiftDown;
}