#include "LoggerManager.h"
#include "Core.h"
#include "configOperate.h"
#include <windows.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <QDir>
#include <QMessageBox>
#include <QtWidgets/QApplication>
#include <QDebug>
#include <QTranslator>
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>

std::shared_ptr<spdlog::logger> LoggerManager::logger_ = nullptr;

void LoggerManager::init()
{

    if (logger_ != nullptr)
    {
        return; // Already initialized
    }
    VCCore::mainConfig = readJSON(MAIN_JSON_FILE);
    QJsonObject cfg = VCCore::mainConfig.object().value("log").toObject();
    if (VCCore::mainConfig.isEmpty())
    {
        OutputDebugString(TEXT("error: is empty!\n"));
    }
    try
    {
        if (!createLogDirectory(VCCore::getPath(cfg.value("logFolder").toString()).toStdString()))
        {
            std::cerr << "Failed to create log directory: " << VCCore::getPath(cfg.value("logFolder").toString()).toStdString() << std::endl;
            std::cerr << "Failed to initialize logger!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        std::string file_pattern = VCCore::getPath(cfg.value("logFolder").toString()).toStdString() + "/" + cfg.value("logName").toString().toStdString() + ".log";

        logger_ = spdlog::rotating_logger_mt("file_logger", file_pattern,
            cfg.value("maxFileSize").toInt(), cfg.value("maxFiles").toInt());

        if (cfg.value("level").toString() == "trace")
            logger_->set_level(spdlog::level::trace);
        else if (cfg.value("level").toString() == "debug")
            logger_->set_level(spdlog::level::debug);
        else if (cfg.value("level").toString() == "warn")
            logger_->set_level(spdlog::level::warn);
        else if (cfg.value("level").toString() == "error")
            logger_->set_level(spdlog::level::err);
        else if (cfg.value("level").toString() == "critical")
            logger_->set_level(spdlog::level::critical);
        else
            logger_->set_level(spdlog::level::info);

        logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
        logger_->flush_on(spdlog::level::warn);
        spdlog::set_default_logger(logger_);
        return;
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        std::cerr << "Failed to initialize logger!" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

std::shared_ptr<spdlog::logger> LoggerManager::getLogger()
{
    if (!logger_)
    {
        throw std::runtime_error("Logger not initialized. Call init first.");
    }
    return logger_;
}

void LoggerManager::shutdown()
{
    if (logger_)
    {
        logger_->flush();
        spdlog::drop_all();
    }
}

bool LoggerManager::createLogDirectory(const std::string& dir_path)
{
    try
    {
        if (!std::filesystem::exists(dir_path))
        {
            return std::filesystem::create_directories(dir_path);
        }
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to create log directory: " << e.what() << std::endl;
        return false;
    }
}