//#include "VoidCrawler.h"
//#include <QApplication>
//#include <QWidget>
//#include <QGraphicsDropShadowEffect>
//#include <QDir>
//#include <QtWidgets/QPushButton>
//#include <QFont>
//
//
//VoidCrawler::VoidCrawler(QWidget* parent) : QMainWindow(parent)
//{
//    ui.setupUi(this);
//
//    // 初始化多语言翻译器
//    translator = new QTranslator(this);
//
//    // 之后应该更改语言、初始化ui等
//}
//
//// 初始化UI界面函数
//void VoidCrawler::initUI()
//{
//    //this->acceptDrops(); // 允许拖放文件，但这是很久以后的功能了
//    this->setWindowFlags(Qt::FramelessWindowHint);
//    this->resize(300, 800);
//    this->setWindowTitle("VoidCrawler Client");
//    this->setWindowIcon(QIcon(QDir::currentPath() + "/icon.ico"));
//
//    // 将窗口主题色改为黑色
//    // 注意！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！ 
//    // 以下这行代码会导致QLabel阴影效果失效！
//    this->setStyleSheet("QMainWindow { background-color: #000000; }");
//    // 创建第一个QLabel
//    QLabel* firstLabel = new QLabel(tr("你好，世界！"), this);
//    // 设置文字颜色并确保背景透明以便阴影只围绕文字显示
//    firstLabel->setStyleSheet("color: rgb(255, 255, 255); background: transparent;");
//    firstLabel->setAttribute(Qt::WA_TranslucentBackground, true);
//    firstLabel->setGeometry(50, 50, 200, 30);  // 设置位置和大小
//
//    // 为 label 使用专属的阴影效果（不可与其他控件共享）
//    QGraphicsDropShadowEffect* labelShadow = new QGraphicsDropShadowEffect(firstLabel);
//    labelShadow->setColor(QColor(138, 0, 255)); // 紫色
//    labelShadow->setOffset(0, 0);
//    // 文字发光通常需要较小到中等的模糊半径
//    labelShadow->setBlurRadius(50);
//    firstLabel->setGraphicsEffect(labelShadow);
//    firstLabel->show();  // 显示QLabel
//
//    // 重新创建按钮并为其使用独立的阴影效果，避免共享导致的问题
//    QPushButton* btn = new QPushButton(tr("UAC ByPass"), this);
//    btn->setGeometry(50, 110, 200, 34);  // 设置位置和大小
//    // 按钮使用深色背景以在黑色主题下保持可见
//    btn->setStyleSheet("QPushButton { background-color: #111111; color: white; border: 1px solid #2c2c2c; border-radius: 4px; }");
//
//    QGraphicsDropShadowEffect* btnShadow = new QGraphicsDropShadowEffect(btn);
//    btnShadow->setColor(QColor(138, 0, 255, 180));
//    btnShadow->setOffset(0, 0);
//    // 按钮阴影更柔和但仍明显
//    btnShadow->setBlurRadius(40);
//    btn->setGraphicsEffect(btnShadow);
//
//    btn->show();  // 显示QPushButton
//}
//
//VoidCrawler::~VoidCrawler()
//{
//    delete translator; // 虽然没有使用智能指针，但也是安全的
//}
//
//// 更改客户端语言，注意：更改语言后需要重新启动客户端以应用更改。（刚启动软件时除外）
//void VoidCrawler::changeLanguage(const QString& language)
//{
//    // 卸载当前翻译器
//    QApplication::removeTranslator(translator);
//
//    // 加载新的翻译文件
//    if (translator->load(QString(":/translations/%1.qm").arg(language)))
//    {
//        QApplication::installTranslator(translator);
//    }
//}