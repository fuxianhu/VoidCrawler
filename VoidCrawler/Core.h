#pragma once

#include "LoggerManager.h"
#include "configOperate.h"
#include "CustomQtLogger.h"
#include "ClientInfo.h"

#include <string>
#include <type_traits>
#include <format>
#include <limits>
#include <memory>
#include <unordered_map>
#include <windows.h>
#include <shellapi.h>
#include <cstdlib>
#include <iostream>

#include <QString>
#include <QDir>
#include <QDateTime>
#include <QCoreApplication>
#include <QtGlobal>
#include <QMessageBox>
#include <QtWidgets/QApplication>
#include <QDebug>
#include <QTranslator>
#include <QSplashScreen>
#include <QObject>
#include <QDateTime>
#include <QTimeZone>
#include <QMainWindow>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>


namespace VCCore
{
    // 主窗口指针
    inline QMainWindow* mainWindow = nullptr;

    // 获取路径字符串
    template <typename StringType>
    inline QString getPath(const StringType& path)
    {
        QString qstr;
        if constexpr (std::is_same_v<StringType, std::string>)
        {
            qstr = QString::fromStdString(path);
        }
        else if constexpr (std::is_same_v<StringType, QString>)
        {
            qstr = path;
        }
        else
        {
            qstr = QString::fromUtf8(reinterpret_cast<const char*>(path));
        }

//#ifdef QT_DEBUG
        return QDir::currentPath() + "/" + qstr;
//#else
//        return QCoreApplication::applicationDirPath() + "/" + qstr;
//#endif
    }

    // 版本描述类，格式为 x.x.x.x，且支持记录描述信息和构建时间、比较大小等
    class Version
    {
    public:
        // quint16 范围 : 0 - 65535

        // 主版本
        const quint16 MAJOR;

        // 次版本
        const quint16 MINOR;

        // 修订版本
        const quint16 PATCH;

        // 构建版本
        const quint16 BUILD;

        // 描述
        const QString DESCRIPTION;

        // 构建时间
        const QDateTime BUILD_TIME;

        Version(const quint16& major = 0,
            const quint16& minor = 0,
            const quint16& patch = 0,
            const quint16& build = 0,
            const QString& description = "",
            const QDateTime& buildTime = QDateTime::currentDateTimeUtc()) : MAJOR(major), MINOR(minor), PATCH(patch), BUILD(build), DESCRIPTION(description), BUILD_TIME(buildTime)
        {
        }

        auto operator<=>(const Version& other) const
        {
            if (auto cmp = MAJOR <=> other.MAJOR; cmp != 0)
                return cmp;
            if (auto cmp = MINOR <=> other.MINOR; cmp != 0)
                return cmp;
            if (auto cmp = PATCH <=> other.PATCH; cmp != 0)
                return cmp;
            return BUILD <=> other.BUILD;
        }

        auto operator==(const Version& other) const
        {
            return MAJOR == other.MAJOR &&
                MINOR == other.MINOR &&
                PATCH == other.PATCH &&
                BUILD == other.BUILD;
        }

        // 转换为 std::string 类型，格式："v.x.x.x.x"
        std::string toStdString() const
        {
            return std::format("v.{}.{}.{}.{}", MAJOR, MINOR, PATCH, BUILD);
        }

        // 转换为 QString 类型，格式："v.x.x.x.x"
        QString toQString() const
        {
            return QString::fromStdString(toStdString());
        }

        // 解析版本字符串，格式为 "v.x.x.x.x" 或 "x.x.x.x"
        template <typename StringType>
        static Version fromString(const StringType& versionStr, QString DESCRIPTION="", QDateTime BUILD_TIME= QDateTime())
        {
            QString str;
            if constexpr (std::is_same_v<StringType, std::string>)
            {
                str = QString::fromStdString(versionStr);
            }
            else if constexpr (std::is_same_v<StringType, QString>)
            {
                str = versionStr;
            }
            else
            {
                str = QString::fromUtf8(reinterpret_cast<const char*>(versionStr));
            }
            if (str.isNull())
            {
                return Version(0, 0, 0, 0, STRING_IS_NULL);
            }
            if (str.startsWith("v.", Qt::CaseInsensitive)) // 字符串以 v. 开头，不区分大小写
            {
                str.remove(0, 2);
            }
            bool flag = true;
            quint16 result[4]{};
            for (int i = 0; i < 4; i++)
            {
                QString s = str.section(".", i, i);
                if (s.isEmpty())
                {
                    flag = false;
                    break;
                }
                for (const QChar& ch : s)
                {
                    if (!ch.isDigit())
                    {
                        flag = false;
                        break;
                    }
                }
                if (!flag)
                {
                    break;
                }
                bool ok;
                uint uIntResult = s.toUInt(&ok);
                if (ok && !s.isEmpty())
                {
                    // auto major = str;
                    if (uIntResult <= (std::numeric_limits<quint16>::max)())
                    {
                        result[i] = static_cast<quint16>(uIntResult);
                    }
                    else
                    {
                        return Version(0, 0, 0, 0, STRING_OTHER_ERROR);
                    }
                }
                else
                {
                    flag = false;
                    break;
                }
            }
            if (!flag)
            {
                return Version(0, 0, 0, 0, STRING_IS_NOT_DIGIT);
            }
            return Version(result[0], result[1], result[2], result[3]);
        }
    };

    // 获取当前客户端发布 UTC 时间
    inline QDateTime getClientBuildUTCTime()
    {
        // 创建带时区的日期时间
        QDateTime localTime(QDate(CLIENT_BUILD_DATETIME_YEAR, CLIENT_BUILD_DATETIME_MONTH, CLIENT_BUILD_DATETIME_DAY), 
            QTime(CLIENT_BUILD_DATETIME_HOUR, CLIENT_BUILD_DATETIME_MINUTE));
        localTime.setTimeZone(QTimeZone(CLIENT_BUILD_DATETIME_TIMEZONE));
        return localTime.toUTC();
    }

	// VoidCrawler 版本信息
    inline Version VoidCrawlerVersion = Version::fromString(CLIENT_VERSION_NUMBER, CLIENT_VERSION_TYPE, getClientBuildUTCTime());

    // 隐藏所有顶级窗口（包括主窗口、独立对话框等）
    void hideAllTopLevelWindows();

    // 显示所有顶级窗口（包括主窗口、独立对话框等）
    void showAllTopLevelWindows();

    // 创建或获取文件夹：目录不存在时创建，存在时直接访问，QDir 函数
    static QDir createAndGetFolder(QDir directory, const QString& folderName)
    {
        if (!directory.cd(folderName))
        {
            OutputDebugString(reinterpret_cast<LPCWSTR>(QString("%1 folder does not exist. Creating...\n").arg(folderName).toStdWString().c_str()));
            if (!directory.mkdir(folderName))
            {
                std::cerr << std::format("Failed to create {} directory!", folderName.toStdString()) << std::endl;
                QMessageBox::critical(nullptr,
                    "VoidCrawler",
                    QString("Failed to create %1 directory!").arg(folderName));
                std::exit(EXIT_FAILURE);
            }
            if (!directory.cd(folderName))
            {
                std::cerr << "Failed to access log directory after creation!" << std::endl;
                QMessageBox::critical(nullptr,
                    "VoidCrawler",
                    "Failed to access log directory after creation!");
                std::exit(EXIT_FAILURE);
            }
        }
        return directory;
    }

    // 初始化日志记录器指针
    static std::shared_ptr<spdlog::logger> initLogger()
    {
        LoggerManager::init();
        return LoggerManager::getLogger();
    }

    // 日志记录器指针
    static std::shared_ptr<spdlog::logger> logger = initLogger();

    // main.json 配置文件的 Doc
    static QJsonDocument mainConfig = readJSON(MAIN_JSON_FILE);

    // category.json 配置文件的 Doc
    static QJsonDocument categoryConfig = readJSON(CATEGORY_JSON_FILE);

    static QJsonDocument itemConfiguration = readJSON(ITEM_CONFIGURATION_JSON_FILE);

    // 使用 ShellExecute 启动外部程序，非阻塞
    void startShellDetached(const QString& program, const QString& arguments = nullptr);

    template <typename StringType>
    inline QJsonValue cfgValue(const StringType& path, const QJsonDocument& cfg)
    {
        // 1. 转换为 QString
        QString qstr;
        if constexpr (std::is_same_v<StringType, std::string>)
        {
            qstr = QString::fromStdString(path);
        }
        else if constexpr (std::is_same_v<StringType, QString>)
        {
            qstr = path;
        }
        else
        {
            qstr = QString::fromUtf8(reinterpret_cast<const char*>(path));
        }

        if (qstr.isEmpty())
        {
            return QJsonValue();
        }

        // 2. 分割路径（处理边界情况）
        const QStringList parts = qstr.split('/', Qt::SkipEmptyParts);
        if (parts.isEmpty())
        {
            return QJsonValue();
        }

        // 3. 遍历路径，每一步都检查有效性
        QJsonValue current = cfg.object().value(parts[0]);

        for (int i = 1; i < parts.size() && current.isObject(); ++i)
        {
            current = current.toObject().value(parts[i]);
        }

        return current;
    }

    template <typename StringType>
    inline QJsonValue mainCfgValue(const StringType& path)
    {
        return cfgValue(path, VCCore::mainConfig);
    }

    template <typename StringType>
    inline QJsonValue itemsCfgValue(const StringType& path)
    {
        return cfgValue(path, VCCore::categoryConfig);
    }
}