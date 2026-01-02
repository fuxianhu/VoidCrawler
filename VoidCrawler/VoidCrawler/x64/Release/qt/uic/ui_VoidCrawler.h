/********************************************************************************
** Form generated from reading UI file 'VoidCrawler.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VOIDCRAWLER_H
#define UI_VOIDCRAWLER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

QT_BEGIN_NAMESPACE

class Ui_VoidCrawlerClass
{
public:

    void setupUi(QMainWindow *VoidCrawlerClass)
    {
        if (VoidCrawlerClass->objectName().isEmpty())
            VoidCrawlerClass->setObjectName("VoidCrawlerClass");
        VoidCrawlerClass->resize(600, 400);

        retranslateUi(VoidCrawlerClass);

        QMetaObject::connectSlotsByName(VoidCrawlerClass);
    } // setupUi

    void retranslateUi(QMainWindow *VoidCrawlerClass)
    {
        VoidCrawlerClass->setWindowTitle(QCoreApplication::translate("VoidCrawlerClass", "VoidCrawler", nullptr));
    } // retranslateUi

};

namespace Ui {
    class VoidCrawlerClass: public Ui_VoidCrawlerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VOIDCRAWLER_H
