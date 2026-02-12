#include "keyhookthread.h"
#include <QDebug>
#include <QCoreApplication>

// 初始化静态成员
KeyHookThread* KeyHookThread::s_instance = nullptr;

KeyHookThread::KeyHookThread(QObject* parent)
    : QThread(parent), m_stopRequested(false), m_keyboardHook(nullptr), m_leftShiftPressed(false), m_rightShiftPressed(false), m_comboTriggered(false)
{
    s_instance = this; // 设置静态实例指针
}

KeyHookThread::~KeyHookThread()
{
    stopHook(); // 确保钩子被卸载
    s_instance = nullptr;
}

void KeyHookThread::stopHook()
{
    m_stopRequested = true;
    // 发送一个虚拟消息来唤醒可能阻塞在GetMessage中的线程
    PostThreadMessage(GetCurrentThreadId(), WM_QUIT, 0, 0);
}

LRESULT CALLBACK KeyHookThread::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && s_instance)
    {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

        // 将按键信息传递给实例函数处理
        bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        s_instance->processKeyEvent(p->vkCode, isKeyDown);
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void KeyHookThread::processKeyEvent(DWORD vkCode, bool isKeyDown)
{
    // 处理左Shift键
    if (vkCode == VK_LSHIFT)
    {
        m_leftShiftPressed = isKeyDown;
        if (!isKeyDown)
        {
            m_comboTriggered = false; // 释放时重置触发标志
        }
    }
    // 处理右Shift键
    else if (vkCode == VK_RSHIFT)
    {
        m_rightShiftPressed = isKeyDown;
        if (!isKeyDown)
        {
            m_comboTriggered = false;
        }
    }

    // 检测组合键：两个Shift键都处于按下状态，且尚未触发
    if (m_leftShiftPressed && m_rightShiftPressed && !m_comboTriggered)
    {
        m_comboTriggered = true; // 设置触发标志，防止重复响应

        // 使用Qt信号将结果发送出去（线程安全）
        emit hotkeyDetected(tr("Hotkey detected"));
        qDebug() << "Hotkey detected in thread:" << QThread::currentThreadId();
    }
}

void KeyHookThread::run()
{
    // 安装全局键盘钩子
    m_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL,
        LowLevelKeyboardProc,
        GetModuleHandle(nullptr),
        0);

    if (!m_keyboardHook)
    {
        DWORD error = GetLastError();
        emit hookError(tr("设置钩子失败！错误代码: %1").arg(error));
        return;
    }

    emit hotkeyDetected(tr("钩子已安装，开始监听左Shift + 右Shift..."));

    // 线程消息循环（阻塞在这里，直到收到退出信号）
    MSG msg;
    while (!m_stopRequested && GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 清理：卸载钩子
    if (m_keyboardHook)
    {
        UnhookWindowsHookEx(m_keyboardHook);
        m_keyboardHook = nullptr;
        emit hotkeyDetected(tr("钩子已卸载"));
    }

    qDebug() << "Hook thread finished";
}