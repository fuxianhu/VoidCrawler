#ifndef GLOBAL_HOTKEY_H
#define GLOBAL_HOTKEY_H

#include <windows.h>
#include <string>
#include <functional>

// 热键模式枚举
enum class HotkeyMode {
    RegisterHotKey = 0,   // 使用RegisterHotKey API
    LowLevelHook = 1      // 使用低级键盘钩子
};

// 回调函数类型
using HotkeyCallback = std::function<void()>;

class GlobalHotKeyDetector {
public:
    // 构造函数
    GlobalHotKeyDetector(HotkeyMode mode = HotkeyMode::RegisterHotKey);
    
    // 析构函数
    ~GlobalHotKeyDetector();
    
    // 设置热键组合 (对于RegisterHotKey模式需要指定一个字符键)
    bool SetHotkey(UINT vk = 0, HotkeyMode mode = HotkeyMode::RegisterHotKey);
    
    // 启动热键检测
    bool StartDetection(HWND hWnd = nullptr, HotkeyCallback callback = nullptr);
    
    // 停止热键检测
    void StopDetection();
    
    // 处理窗口消息 (用于RegisterHotKey模式)
    void ProcessWindowMessage(UINT message, WPARAM wParam, LPARAM lParam);
    
    // 获取模式
    HotkeyMode GetMode() const { return m_mode; }
    
    // 获取状态
    bool IsRunning() const { return m_isRunning; }
    
    // 检查热键是否可用
    static bool IsHotkeyAvailable(UINT modifiers, UINT vk);
    
private:
    // 初始化钩子 (LowLevelHook模式)
    bool InstallHook();
    
    // 卸载钩子
    void UninstallHook();
    
    // 注册热键 (RegisterHotKey模式)
    bool RegisterSystemHotkey();
    
    // 注销热键
    void UnregisterSystemHotkey();
    
    // 钩子回调函数 (静态)
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    
    // 处理按键组合 (用于钩子模式)
    void ProcessKeyCombination(UINT vkCode, bool keyDown);
    
    // 检查三个修饰键是否同时按下
    bool CheckModifierKeys();
    
private:
    // 热键配置
    UINT m_virtualKey;          // 虚拟键码 (如 'A', 'S' 等)
    UINT m_modifiers;           // 修饰键组合
    HotkeyMode m_mode;          // 检测模式
    
    // 运行状态
    bool m_isRunning;
    bool m_hotkeyRegistered;
    
    // 窗口相关
    HWND m_hWnd;                // 接收消息的窗口句柄
    int m_hotkeyId;             // 热键ID
    
    // 钩子相关
    HHOOK m_keyboardHook;       // 键盘钩子句柄
    
    // 回调函数
    HotkeyCallback m_callback;
    
    // 静态实例指针 (用于钩子回调)
    static GlobalHotKeyDetector* s_instance;
    
    // 按键状态跟踪 (用于钩子模式)
    bool m_ctrlPressed;
    bool m_winPressed;
    bool m_shiftPressed;
    bool m_targetKeyPressed;
};

#endif // GLOBAL_HOTKEY_H