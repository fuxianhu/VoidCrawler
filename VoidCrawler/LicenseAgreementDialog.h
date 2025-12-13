#pragma once

#include <QDialog>
#include <QTextBrowser>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QMessageBox>
#include <QApplication>
#include <QFile>
#include <QScrollBar>
#include <QTextStream>
#include <QStringConverter>
#include <QEvent>

/**
 * @brief 使用条款同意对话框（简化版）
 *
 * 这个对话框显示使用条款并要求用户同意。条款以Markdown格式显示。
 * 简洁UI设计：只有文字、超链接和两个按钮。
 */
class LicenseAgreementDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口部件
     */
    explicit LicenseAgreementDialog(QWidget* parent = nullptr);

    /**
     * @brief 获取用户是否同意了条款
     * @return 如果用户同意了条款返回true，否则返回false
     */
    bool isLicenseAccepted() const { return m_accepted; }

    /**
     * @brief 从文件加载Markdown格式的条款内容
     * @param filePath 文件路径
     * @return 如果加载成功返回true，否则返回false
     */
    bool loadLicenseFromFile(const QString& filePath);

    /**
     * @brief 设置Markdown格式的条款内容
     * @param markdownText Markdown格式的文本
     */
    void setLicenseMarkdown(const QString& markdownText);

    /**
     * @brief 获取当前的条款内容
     * @return Markdown格式的条款文本
     */
    QString licenseMarkdown() const { return m_licenseMarkdown; }

protected:
    /**
     * @brief 关闭事件处理，防止用户直接关闭窗口
     */
    void closeEvent(QCloseEvent* event) override;

    /**
     * @brief 事件过滤器，用于处理超链接点击
     */
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onViewLicenseClicked();
    void onAcceptClicked();
    void onRejectClicked();

private:
    void setupUI();
    void setupStyles();
    void createConnections();
    QString getDefaultLicenseMarkdown();

    // UI组件
    QLabel* m_titleLabel;
    QLabel* m_licenseLinkLabel;
    QPushButton* m_acceptBtn;
    QPushButton* m_rejectBtn;

    // 条款查看对话框
    QDialog* m_licenseViewer;
    QTextBrowser* m_licenseBrowser;

    // 状态
    bool m_accepted;
    QString m_licenseMarkdown;
};