#pragma once

#include <spdlog/logger.h>
#include <QDebug>

class CustomQtLogger : public spdlog::logger {
public:
    using spdlog::logger::logger;

    template<typename... Args>
    void trace(const char* fmt, const Args&... args) {
        spdlog::logger::debug(fmt, args...);
        qDebug().noquote() << QString::asprintf(fmt, args...);
    }

    template<typename... Args>
    void debug(const char* fmt, const Args&... args) {
        spdlog::logger::debug(fmt, args...);
        qDebug().noquote() << QString::asprintf(fmt, args...);
    }

    template<typename... Args>
    void info(const char* fmt, const Args&... args) {
        spdlog::logger::info(fmt, args...);
        qInfo().noquote() << QString::asprintf(fmt, args...);
    }

    template<typename... Args>
    void warn(const char* fmt, const Args&... args) {
        spdlog::logger::warn(fmt, args...);
        qWarning().noquote() << QString::asprintf(fmt, args...);
    }

    template<typename... Args>
    void error(const char* fmt, const Args&... args) {
        spdlog::logger::error(fmt, args...);
        qCritical().noquote() << QString::asprintf(fmt, args...);
    }

    template<typename... Args>
    void critical(const char* fmt, const Args&... args) {
        spdlog::logger::critical(fmt, args...);
        qFatal().noquote() << QString::asprintf(fmt, args...);
    }
};