#include "LicenseAgreementDialog.h"

LicenseAgreementDialog::LicenseAgreementDialog(QWidget* parent)
    : QDialog(parent)
    , m_accepted(false)
{
    // 设置窗口属性
    setWindowTitle(tr("Terms of Use"));
    setModal(true);
    setFixedSize(500, 220);

    // 加载默认条款
    m_licenseMarkdown = getDefaultLicenseMarkdown();

    // 初始化UI
    setupUI();
    setupStyles();
    createConnections();
}

void LicenseAgreementDialog::setupUI()
{
    // 设置窗口背景色
    setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor("#f8f9fa"));
    setPalette(palette);

    // 主对话框布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 30);

    // 标题文字
    m_titleLabel = new QLabel(tr("Do you agree to the Terms of Use?"), this);
    m_titleLabel->setWordWrap(true);

    QFont titleFont("Segoe UI", 13, QFont::Normal);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("color: #2c3e50; padding: 10px;");

    // 条款链接
    QString linkText = tr("<a href=\"#\" style=\"color: #3498db; text-decoration: none;\">View the Terms of Use</a>");
    m_licenseLinkLabel = new QLabel(linkText, this);
    m_licenseLinkLabel->setAlignment(Qt::AlignCenter);
    m_licenseLinkLabel->setCursor(Qt::PointingHandCursor);
    m_licenseLinkLabel->setOpenExternalLinks(false);

    QFont linkFont("Segoe UI", 11);
    m_licenseLinkLabel->setFont(linkFont);

    // 按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_rejectBtn = new QPushButton(tr("Disagree"), this);
    m_acceptBtn = new QPushButton(tr("Agree"), this);

    // 设置按钮最小尺寸
    m_rejectBtn->setMinimumSize(100, 40);
    m_acceptBtn->setMinimumSize(100, 40);

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_rejectBtn);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(m_acceptBtn);
    buttonLayout->addStretch();

    // 添加到主布局
    mainLayout->addWidget(m_titleLabel);
    mainLayout->addSpacing(15);
    mainLayout->addWidget(m_licenseLinkLabel);
    mainLayout->addSpacing(30);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();

    // ====== 创建条款查看对话框 ======
    m_licenseViewer = new QDialog(this);
    m_licenseViewer->setWindowTitle(tr("Terms of Use"));
    m_licenseViewer->resize(750, 550);

    QVBoxLayout* viewerLayout = new QVBoxLayout(m_licenseViewer);
    viewerLayout->setContentsMargins(0, 0, 0, 0);

    // 条款浏览器 - 支持Markdown
    m_licenseBrowser = new QTextBrowser(m_licenseViewer);
    m_licenseBrowser->setMarkdown(m_licenseMarkdown);
    m_licenseBrowser->setOpenExternalLinks(true);
    m_licenseBrowser->setReadOnly(true);

    // 设置浏览器样式
    m_licenseBrowser->setStyleSheet(R"(
        QTextBrowser {
            background-color: white;
            border: none;
            padding: 20px;
            font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
            font-size: 12pt;
            line-height: 1.6;
        }
        QTextBrowser a {
            color: #3498db;
            text-decoration: none;
        }
        QTextBrowser a:hover {
            text-decoration: underline;
        }
        QScrollBar:vertical {
            background-color: #f1f1f1;
            width: 12px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background-color: #c1c1c1;
            border-radius: 6px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #a8a8a8;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
    )");

    viewerLayout->addWidget(m_licenseBrowser);

    // 安装事件过滤器来处理超链接点击
    m_licenseLinkLabel->installEventFilter(this);
}

void LicenseAgreementDialog::setupStyles()
{
    // 主窗口样式
    setStyleSheet(R"(
        QDialog {
            background-color: #f8f9fa;
            border: 1px solid #e0e0e0;
            border-radius: 10px;
        }
        
        QLabel {
            background-color: transparent;
        }
    )");

    // 同意按钮样式
    m_acceptBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #28a745;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 10px 25px;
            font-size: 13px;
            font-weight: 500;
            min-width: 100px;
            min-height: 40px;
        }
        QPushButton:hover {
            background-color: #218838;
        }
        QPushButton:pressed {
            background-color: #1e7e34;
        }
        QPushButton:focus {
            outline: none;
            border: 2px solid rgba(40, 167, 69, 0.5);
        }
    )");

    // 不同意按钮样式
    m_rejectBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #dc3545;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 10px 25px;
            font-size: 13px;
            font-weight: 500;
            min-width: 100px;
            min-height: 40px;
        }
        QPushButton:hover {
            background-color: #c82333;
        }
        QPushButton:pressed {
            background-color: #bd2130;
        }
        QPushButton:focus {
            outline: none;
            border: 2px solid rgba(220, 53, 69, 0.5);
        }
    )");

    // 链接标签悬停效果
    m_licenseLinkLabel->setStyleSheet(R"(
        QLabel:hover {
            background-color: rgba(52, 152, 219, 0.1);
            border-radius: 4px;
        }
    )");
}

void LicenseAgreementDialog::createConnections()
{
    connect(m_acceptBtn, &QPushButton::clicked,
        this, &LicenseAgreementDialog::onAcceptClicked);
    connect(m_rejectBtn, &QPushButton::clicked,
        this, &LicenseAgreementDialog::onRejectClicked);
}

QString LicenseAgreementDialog::getDefaultLicenseMarkdown()
{
    return tr(R"(
### **VoidCrawler 客户端软件最终用户许可协议（EULA）**

**协议生效日期：2025年12月5日
**编写者/提供方：*杨方烨*（亦称 YFY）

---

#### **重要通知——请务必仔细阅读**

在使用、复制、安装或以任何方式操作VoidCrawler客户端软件（以下简称“本软件”）之前，您（“用户”或“您”）必须仔细阅读、理解并同意接受本协议的所有条款。**下载、安装、运行本软件的任一行为，即被视为您已阅读、理解并无条件接受本协议的全部内容，特别是其中以加粗形式提示的关于法律责任豁免、用户行为约束及高风险警示的条款。如果您不同意本协议的任何内容，请立即删除本软件及其所有副本。**

**本软件严格限定于法律授权的安全研究、教学演示及授权测试环境中使用。任何超出此范围的用途，特别是用于攻击、入侵、破坏或规避任何计算机系统、网络、数据或技术保护措施的行为，均可能构成严重的民事侵权或刑事犯罪。软件提供方对此类非法用途及其后果明确不予认可，且不承担任何责任。**

---

#### **第一条 定义**

1.1 **“本软件”**：指由编写者xxx（YFY）开发并提供下载的名为“VoidCrawler”的客户端软件程序，包括其可执行文件、相关库、文档及任何更新版本。
1.2 **“提供方”**：指本软件的编写者与发布者，即xxx（亦称YFY）。
1.3 **“授权环境”**：指用户个人拥有完全所有权的设备，或已获得系统所有者明确、书面授权进行安全测试与评估的环境。

#### **第二条 许可授予与限制**

2.1 **有限许可**：提供方授予您一项个人的、非独占的、不可转让、不可分许可的、可随时撤销的许可，仅限于在**授权环境**内，出于**个人学习、安全研究或法律允许的测试目的**安装和使用本软件的一份副本。
2.2 **严格禁止的用途**：您**不得**，且明确承诺不会将本软件用于以下任何目的，包括但不限于：
    * (a) 未经授权访问、探测、攻击或破坏任何计算机系统、网络、服务器或数据；
    * (b) 规避、破坏或干扰任何软件、系统或网络中内置的技术保护措施、身份验证机制或安全控制；
    * (c) 进行任何形式的网络攻击，如分布式拒绝服务攻击、密码爆破、恶意软件传播等；
    * (d) 侵犯任何个人、组织或实体的隐私权、著作权、专利权、商业秘密或其他合法权利；
    * (e) 从事任何违反中华人民共和国《网络安全法》、《数据安全法》、《个人信息保护法》、《刑法》及任何其他适用法律法规的活动。
2.3 **禁止商业用途**：您不得出于任何商业目的销售、出租、租赁、出借本软件或利用本软件提供服务。

#### **第三条 用户义务与承诺**

3.1 **合法性保证**：您声明并保证，您使用本软件的一切行为均符合您所在地及行为发生地的所有法律法规，并已获得所有必要的授权。
3.2 **责任自负**：您完全理解并同意，因使用本软件（无论是否符合本协议）所产生的**一切后果、风险、法律责任（包括民事赔偿、行政处罚及刑事责任）将由您独自承担**。
3.3 **损害赔偿**：若因您违反本协议或法律法规使用本软件，导致提供方遭受任何第三方提起的索赔、诉讼或产生任何损失（包括律师费、诉讼费、赔偿金），您有义务为提供方进行辩护，并赔偿其全部损失。

#### **第四条 知识产权声明**

4.1 **限制**：您不得违反软件的开源许可协议。

#### **第五条 隐私与数据**

5.1 **无数据收集**：提供方郑重声明，本软件**不主动收集、传输或存储**任何用户的个人身份信息、设备信息或使用数据。
5.2 **用户操作风险**：然而，您在使用本软件过程中，可能主动向第三方目标系统发送数据或留下日志痕迹。**此等行为完全由您自行发起与控制，提供方不对该等数据的去向、安全性及因此引发的任何隐私泄露或法律问题负责。**
5.3 **第三方风险**：您通过本软件与之交互的第三方系统，其隐私政策及数据处理行为由该第三方负责，与提供方无关。

#### **第六条 免责声明与责任限制（核心条款）**

**6.1 “按现状”提供**：本软件在“**按其现状**”和“**可能存在瑕疵**”的基础上提供。提供方**明确拒绝**就本软件的适销性、特定用途适用性、不侵权性及安全性作出任何明示或默示的担保。
**6.2 责任豁免**：在法律允许的最大范围内，提供方及其关联方在任何情况下，均不对因使用或无法使用本软件而引起的或与之相关的**任何直接、间接、附带、特殊、惩罚性或后果性损害**（包括但不限于数据丢失、利润损失、业务中断、设备损坏、人身伤害、隐私泄露及因违法所导致的罚款、诉讼费用）承担任何责任，无论此责任基于合同、侵权（包括过失）、严格责任或其他理论，也无论提供方是否已被事先告知发生此类损害的可能性。
**6.3 最终责任**：用户是本软件使用的**最终且唯一**的责任主体。本协议的所有条款，尤其是本免责条款，旨在为善意研究者在法律边缘明确风险界限，而非为非法行为提供庇护。

#### **第七条 协议变更与终止**

7.1 **变更**：提供方保留随时单方面修改、更新本协议的权利。修改后的协议将在提供方指定的渠道公布后立即生效。您继续使用本软件即视为接受修改后的协议。
7.2 **终止**：您违反本协议任何条款，或提供方自行决定，均可导致本协议立即自动终止，且无需通知。协议终止后，您必须立即销毁本软件的所有副本。

#### **第八条 其他条款**

8.1 **可分性**：若本协议任何条款被认定为无效或不可执行，该条款应在最小必要范围内受限，不影响其余条款的持续有效性和执行力。
8.2 **完整协议**：本协议构成您与提供方之间就本软件所涉事项的完整协议，并取代所有先前的口头或书面沟通。
8.3 **管辖法律**：本协议的订立、执行和解释均适用中华人民共和国法律（为本协议之目的，不包括香港、澳门和台湾地区法律）。
8.4 **争议解决**：因本协议引起的或与之相关的任何争议，双方应首先友好协商解决；协商不成的，任何一方均有权将争议提交至**提供方所在地有管辖权的人民法院**通过诉讼解决。

---

**再次警告：本软件涉及高风险。您点击“同意”或使用本软件的行为，代表您已充分理解并接受：您将独自承担全部法律与道德风险，并永久豁免提供方的一切责任。请谨慎抉择。**

---
)");
}

void LicenseAgreementDialog::onViewLicenseClicked()
{
    // 更新浏览器内容
    m_licenseBrowser->setMarkdown(m_licenseMarkdown);
    m_licenseBrowser->verticalScrollBar()->setValue(0);

    // 显示模态对话框
    m_licenseViewer->exec();
}

void LicenseAgreementDialog::onAcceptClicked()
{
    m_accepted = true;
    accept();
}

void LicenseAgreementDialog::onRejectClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Confirm Exit"),
        tr("You need to agree to the Terms of Use to use this software.\n\n"
            "Select 'Yes' to exit the program."),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        m_accepted = false;
        reject();
		QApplication::quit();
    }
}

bool LicenseAgreementDialog::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_licenseLinkLabel && event->type() == QEvent::MouseButtonRelease) {
        onViewLicenseClicked();
        return true;
    }
    return QDialog::eventFilter(obj, event);
}

void LicenseAgreementDialog::closeEvent(QCloseEvent* event)
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Confirm Exit"),
        tr("You need to agree to the Terms of Use to use this software.\n"
            "Are you sure you want to exit?"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        event->accept();
        qApp->quit();
    }
    else {
        event->ignore();
    }
}

bool LicenseAgreementDialog::loadLicenseFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Load Failed"),
            tr("Cannot open file: %1").arg(filePath));
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    m_licenseMarkdown = in.readAll();
    file.close();

    return true;
}

void LicenseAgreementDialog::setLicenseMarkdown(const QString& markdownText)
{
    m_licenseMarkdown = markdownText;
    if (m_licenseBrowser) {
        m_licenseBrowser->setMarkdown(m_licenseMarkdown);
    }
}