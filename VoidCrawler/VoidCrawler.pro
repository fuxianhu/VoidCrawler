# VoidCrawler.pro
TEMPLATE = app
TARGET = VoidCrawler

# 源文件
SOURCES += main.cpp \
    VoidCrawler.cpp \
    LoggerManger.cpp \
    configOperate.cpp

# 头文件
HEADERS += VoidCrawler.h \
    LoggerManager.h

# UI文件
FORMS += VoidCrawler.ui

# 翻译文件
TRANSLATIONS += translations/zh-CN.ts