#pragma once

#include "keyhookthread.h"
#include <QMainWindow>
#include <QTranslator>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPoint>
#include <QResizeEvent>
#include <QEvent>
#include <QSplashScreen>
#include "ui_VoidCrawler.h"


class VoidCrawler : public QMainWindow
{
    Q_OBJECT

public:
    // 构造函数：初始化 UI 相关成员
    explicit VoidCrawler(QWidget* parent = nullptr);

    // 析构函数：释放翻译器对象
    ~VoidCrawler();

    // 初始化 UI 界面函数：创建标题栏、折叠内容区及动画
    void initUI();

    // 切换语言：卸载并重新加载指定语言的翻译文件（需要在外部重启程序以完全应用）
    void changeLanguage(const QString& language);

protected:
    // 重写 resizeEvent：在窗口大小变化时更新子控件几何并重新生成圆角遮罩
    void resizeEvent(QResizeEvent* event) override;

    // 事件过滤器：处理标题栏的鼠标拖动逻辑，从而实现无边框窗口的拖动行为
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    // 检查配置文件
    void checkConfigFiles();

    // 客户端锁，若失败直接记录日志并 std::exit(0);
    // 注意：尝试次数达到限制时会强制锁定
    void clientLock();

    // 要求同意使用条款
    void agreeTerms();

    // 客户端（允许/禁止）多开处理
    void multipleApplicationsProcessing();

	// 显示权限图标
    void privilegeIconDisplay();

    // 启动后的一些处理，调用 initUI() 时立即调用
    void ProgramAfterStartup();

	// 按钮点击处理槽函数
	void on_button_clicked(QString id);

    // 全局热键槽函数
    void onHotkeyDetected(const QString& message);

	// 钩子错误处理槽函数
    void onHookError(const QString& error);

    KeyHookThread* m_hookThread; // 钩子线程指针
    QLabel* m_statusLabel;       // 用于显示状态的标签

    QLabel* privilegeIcon = nullptr;

    Ui::VoidCrawlerClass ui;
    QTranslator* translator = nullptr;
    int m_cornerRadius = 12;

    QWidget* m_titleBar = nullptr;
    QLabel* m_titleLabel = nullptr;
    QWidget* m_contentWidget = nullptr;

    bool m_dragging = false;
    QPoint m_dragPosition;

    QPushButton* m_toggleButton = nullptr;
    bool m_expanded = true;

    int m_expandedHeight = 800;
    int m_collapsedHeight = 100;
    int m_contentExpandedHeight = 0;
    int m_contentCollapsedHeight = 0;
};