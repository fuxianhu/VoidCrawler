#include "Core.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <algorithm>
#include <filesystem>
#include <iostream>

#include <unordered_map>
#include <windows.h>
#include <cstdlib>
#include <format>

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
#include <QFile>
#include <QTextStream>


QJsonDocument readJSON(const QString& path,
    const QStringConverter::Encoding& encoding,
    const bool& useConfigFolder)
{
    // 自动生成 JSON 文件路径
    QString filePath = useConfigFolder ? (VCCore::getPath("config") + "/" + path) : path;
    QFile jsonFile(filePath);
    if (jsonFile.exists())
    {
        // 如果文件存在，读取配置
        if (!jsonFile.open(QFile::ReadOnly | QFile::Text)) {
            VCCore::logger->error("JSON configuration file read failed! Can't open file!\n");
            OutputDebugString(TEXT("JSON 配置文件读取失败！原因：Can't open file!\n"));
            return QJsonDocument();
        }
        // 读取文件全部内容
        QTextStream stream(&jsonFile);
        stream.setEncoding(encoding); // 设置读取编码
        QString str = stream.readAll();
        jsonFile.close();

        // 解析 JSON 对象
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
        if (jsonError.error != QJsonParseError::NoError && !jsonDoc.isNull()) {
            VCCore::logger->error("JSON configuration file read failed! \n");
            OutputDebugString(TEXT("JSON 配置文件读取失败！\n"));
            return QJsonDocument();
        }
        if (jsonDoc.isNull()) {
            OutputDebugString(TEXT("JSON 配置文件读取失败！原因：doc.isNull()\n"));
        }
        else if (!jsonDoc.isObject())
        {
            OutputDebugString(TEXT("JSON 配置文件读取失败！原因：!doc.isObject()\n"));
        }
        return jsonDoc;
    }
    else
    {
        // 文件不存在
        OutputDebugString(TEXT("JSON 配置文件读取失败！原因：!rj\n"));
        return QJsonDocument();
    }
}

bool writeJSON(const QJsonDocument& doc,
    const QString& path,
    const QStringConverter::Encoding& encoding,
    const bool& useConfigFolder)
{
    // 自动生成 JSON 文件路径
    QString filePath = useConfigFolder ? (VCCore::getPath("config") + "/" + path) : path;
    QFile jsonFile(filePath);
    if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        VCCore::logger->error("JSON configuration file write failed! Can't open file!\n");
        QMessageBox::critical(nullptr,
            "VoidCrawler",
            "Unable to open configuration file!");
        return false;
    }
    QTextStream stream(&jsonFile);
    stream.setEncoding(encoding); // 设置写入编码
    // 写入文件
    stream << doc.toJson();
    jsonFile.close();
    return true;
}