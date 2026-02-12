/*
使用C++语言与Windows API实现一个Auto Click函数，全局热键使用Qt 6.5实现。
全局热键参考代码：(KeyHookThread是我自己写的实现)
    if (!m_hookThread)
    {
        m_hookThread = new KeyHookThread(this);
        connect(m_hookThread, &KeyHookThread::hotkeyDetected, this, &VoidCrawler::onHotkeyDetected);
        connect(m_hookThread, &KeyHookThread::hookError, this, &VoidCrawler::onHookError);
        connect(m_hookThread, &QThread::finished, m_hookThread, &QObject::deleteLater);
        m_hookThread->start();
    }
void VoidCrawler::onHotkeyDetected(const QString& message)
{
    // 在主线程中安全地更新
    if (firstCall)
    {
        firstCall = false;
    }
    else
    {
        if (isDisplay)
        {
            isDisplay = false;
            VCCore::hideAllTopLevelWindows();
        }
        else
        {
            isDisplay = true;
            VCCore::showAllTopLevelWindows();
        }
    }
    VCCore::logger->info(std::format("Main threrad: {} Message: {}",
        QString("0x%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()), 16, 16, QLatin1Char('0')).toStdString(),
        message.toStdString()));
}

void VoidCrawler::onHookError(const QString& error)
{
    m_statusLabel->setText("错误: " + error);
    VCCore::logger->error(std::format("Hook error: {}", error.toStdString()));
}
*/
#pragma once

#include <QThread>
#include <windows.h>

class KeyHookThread : public QThread
{
    Q_OBJECT

public:
    explicit KeyHookThread(QObject* parent = nullptr);
    ~KeyHookThread();

    void stopHook(); // 用于安全停止钩子的公共接口

signals:
    // 自定义信号，用于将检测结果发送给主线程
    void hotkeyDetected(const QString& message);
    void hookError(const QString& error);

protected:
    void run() override; // 线程入口函数

private:
    // 静态函数：钩子回调函数，必须为静态或全局函数
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    // 处理按键检测逻辑的辅助函数
    void processKeyEvent(DWORD vkCode, bool isKeyDown);

    // 成员变量
    std::atomic<bool> m_stopRequested; // 线程安全的状态标志
    HHOOK m_keyboardHook;

    // 静态指针，用于在静态回调函数中访问实例成员
    static KeyHookThread* s_instance;

    // 按键状态
    bool m_leftShiftPressed;
    bool m_rightShiftPressed;
    bool m_comboTriggered; // 防止组合键被连续触发
};