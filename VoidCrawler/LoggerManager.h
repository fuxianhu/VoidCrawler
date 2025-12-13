#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <QtWidgets/QApplication>

class LoggerManager
{
public:
    static void init();
    static std::shared_ptr<class spdlog::logger> getLogger();
    static void shutdown();

private:
    static std::shared_ptr<class spdlog::logger> logger_;
    static bool createLogDirectory(const std::string& dir_path);
};