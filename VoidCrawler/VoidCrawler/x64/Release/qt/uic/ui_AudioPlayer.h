/********************************************************************************
** Form generated from reading UI file 'AudioPlayer.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUDIOPLAYER_H
#define UI_AUDIOPLAYER_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *btnAdd;
    QPushButton *btnRemove;
    QPushButton *btnClear;
    QPushButton *btnClose;
    QSplitter *splitter;
    QListWidget *listWidget;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout;
    QLabel *labPic;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnPlay;
    QPushButton *btnPause;
    QPushButton *btnStop;
    QPushButton *btnPrevious;
    QPushButton *btnNext;
    QSpacerItem *horizontalSpacer;
    QDoubleSpinBox *doubleSpinBox;
    QPushButton *btnLoop;
    QPushButton *btnSound;
    QSlider *sliderVolumn;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labCurMedia;
    QSlider *sliderPosition;
    QLabel *labRatio;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(577, 365);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/images/music24.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        verticalLayout_3 = new QVBoxLayout(centralWidget);
        verticalLayout_3->setSpacing(2);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(5, 5, 5, 5);
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName("groupBox");
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(2, 2, 2, 2);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        btnAdd = new QPushButton(groupBox);
        btnAdd->setObjectName("btnAdd");
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/images/images/316.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        btnAdd->setIcon(icon1);

        horizontalLayout_3->addWidget(btnAdd);

        btnRemove = new QPushButton(groupBox);
        btnRemove->setObjectName("btnRemove");
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/images/images/318.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        btnRemove->setIcon(icon2);

        horizontalLayout_3->addWidget(btnRemove);

        btnClear = new QPushButton(groupBox);
        btnClear->setObjectName("btnClear");
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/images/images/214.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        btnClear->setIcon(icon3);

        horizontalLayout_3->addWidget(btnClear);

        btnClose = new QPushButton(groupBox);
        btnClose->setObjectName("btnClose");
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/images/images/132.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        btnClose->setIcon(icon4);

        horizontalLayout_3->addWidget(btnClose);


        verticalLayout_2->addLayout(horizontalLayout_3);

        splitter = new QSplitter(groupBox);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Horizontal);
        listWidget = new QListWidget(splitter);
        listWidget->setObjectName("listWidget");
        listWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        listWidget->setDragEnabled(true);
        listWidget->setDragDropMode(QAbstractItemView::InternalMove);
        splitter->addWidget(listWidget);
        scrollArea = new QScrollArea(splitter);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setMinimumSize(QSize(200, 0));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 198, 253));
        verticalLayout = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName("verticalLayout");
        labPic = new QLabel(scrollAreaWidgetContents);
        labPic->setObjectName("labPic");
        labPic->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(labPic);

        scrollArea->setWidget(scrollAreaWidgetContents);
        splitter->addWidget(scrollArea);

        verticalLayout_2->addWidget(splitter);

        verticalLayout_2->setStretch(1, 1);

        verticalLayout_3->addWidget(groupBox);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(10);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(5, -1, 5, -1);
        btnPlay = new QPushButton(centralWidget);
        btnPlay->setObjectName("btnPlay");
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/images/images/620.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        btnPlay->setIcon(icon5);

        horizontalLayout->addWidget(btnPlay);

        btnPause = new QPushButton(centralWidget);
        btnPause->setObjectName("btnPause");
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/images/images/622.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        btnPause->setIcon(icon6);

        horizontalLayout->addWidget(btnPause);

        btnStop = new QPushButton(centralWidget);
        btnStop->setObjectName("btnStop");
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/images/images/624.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        btnStop->setIcon(icon7);

        horizontalLayout->addWidget(btnStop);

        btnPrevious = new QPushButton(centralWidget);
        btnPrevious->setObjectName("btnPrevious");
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/images/images/616.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        btnPrevious->setIcon(icon8);

        horizontalLayout->addWidget(btnPrevious);

        btnNext = new QPushButton(centralWidget);
        btnNext->setObjectName("btnNext");
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/images/images/630.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        btnNext->setIcon(icon9);

        horizontalLayout->addWidget(btnNext);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        doubleSpinBox = new QDoubleSpinBox(centralWidget);
        doubleSpinBox->setObjectName("doubleSpinBox");
        doubleSpinBox->setMinimumSize(QSize(0, 24));
        doubleSpinBox->setFrame(true);
        doubleSpinBox->setDecimals(1);
        doubleSpinBox->setMinimum(0.500000000000000);
        doubleSpinBox->setMaximum(1.500000000000000);
        doubleSpinBox->setSingleStep(0.100000000000000);
        doubleSpinBox->setValue(1.000000000000000);

        horizontalLayout->addWidget(doubleSpinBox);

        btnLoop = new QPushButton(centralWidget);
        btnLoop->setObjectName("btnLoop");
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/images/images/refresh16.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnLoop->setIcon(icon10);
        btnLoop->setCheckable(true);
        btnLoop->setChecked(true);

        horizontalLayout->addWidget(btnLoop);

        btnSound = new QPushButton(centralWidget);
        btnSound->setObjectName("btnSound");
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/images/images/volumn.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        btnSound->setIcon(icon11);
        btnSound->setIconSize(QSize(24, 24));
        btnSound->setFlat(true);

        horizontalLayout->addWidget(btnSound);

        sliderVolumn = new QSlider(centralWidget);
        sliderVolumn->setObjectName("sliderVolumn");
        sliderVolumn->setMaximum(100);
        sliderVolumn->setValue(100);
        sliderVolumn->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(sliderVolumn);


        verticalLayout_3->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(9);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        labCurMedia = new QLabel(centralWidget);
        labCurMedia->setObjectName("labCurMedia");
        labCurMedia->setMinimumSize(QSize(100, 0));

        horizontalLayout_2->addWidget(labCurMedia);

        sliderPosition = new QSlider(centralWidget);
        sliderPosition->setObjectName("sliderPosition");
        sliderPosition->setTracking(false);
        sliderPosition->setOrientation(Qt::Horizontal);

        horizontalLayout_2->addWidget(sliderPosition);

        labRatio = new QLabel(centralWidget);
        labRatio->setObjectName("labRatio");
        labRatio->setMinimumSize(QSize(80, 0));

        horizontalLayout_2->addWidget(labRatio);


        verticalLayout_3->addLayout(horizontalLayout_2);

        verticalLayout_3->setStretch(0, 1);
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);
        QObject::connect(btnClose, &QPushButton::clicked, MainWindow, qOverload<>(&QMainWindow::close));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\351\237\263\344\271\220\346\222\255\346\224\276\345\231\250", nullptr));
        groupBox->setTitle(QString());
        btnAdd->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240", nullptr));
        btnRemove->setText(QCoreApplication::translate("MainWindow", "\347\247\273\351\231\244", nullptr));
        btnClear->setText(QCoreApplication::translate("MainWindow", "\346\270\205\347\251\272", nullptr));
        btnClose->setText(QCoreApplication::translate("MainWindow", "\351\200\200\345\207\272", nullptr));
        labPic->setText(QCoreApplication::translate("MainWindow", "Pic", nullptr));
#if QT_CONFIG(tooltip)
        btnPlay->setToolTip(QCoreApplication::translate("MainWindow", "\346\222\255\346\224\276", nullptr));
#endif // QT_CONFIG(tooltip)
        btnPlay->setText(QString());
#if QT_CONFIG(tooltip)
        btnPause->setToolTip(QCoreApplication::translate("MainWindow", "\346\232\202\345\201\234", nullptr));
#endif // QT_CONFIG(tooltip)
        btnPause->setText(QString());
#if QT_CONFIG(tooltip)
        btnStop->setToolTip(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242", nullptr));
#endif // QT_CONFIG(tooltip)
        btnStop->setText(QString());
#if QT_CONFIG(tooltip)
        btnPrevious->setToolTip(QCoreApplication::translate("MainWindow", "\345\211\215\344\270\200\346\233\262", nullptr));
#endif // QT_CONFIG(tooltip)
        btnPrevious->setText(QString());
#if QT_CONFIG(tooltip)
        btnNext->setToolTip(QCoreApplication::translate("MainWindow", "\345\220\216\344\270\200\346\233\262", nullptr));
#endif // QT_CONFIG(tooltip)
        btnNext->setText(QString());
        doubleSpinBox->setPrefix(QString());
        doubleSpinBox->setSuffix(QCoreApplication::translate("MainWindow", " \345\200\215\351\200\237 ", nullptr));
        btnLoop->setText(QCoreApplication::translate("MainWindow", "\345\276\252\347\216\257", nullptr));
        btnSound->setText(QString());
        labCurMedia->setText(QCoreApplication::translate("MainWindow", "\346\227\240\346\233\262\347\233\256", nullptr));
        labRatio->setText(QCoreApplication::translate("MainWindow", "\350\277\233\345\272\246", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUDIOPLAYER_H
