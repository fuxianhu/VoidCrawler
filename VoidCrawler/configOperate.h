#pragma once

#include <QDir>
#include <QString>
#include <QJsonDocument>
#include <QStringConverter>

// 读取 JSON 文件，成功则返回对应的 QJsonDocument，否则返回空的 QJsonDocument
QJsonDocument readJSON(const QString& path,
    const QStringConverter::Encoding& encoding = QStringConverter::Utf8,
    const bool& useConfigFolder = true);

// 写入 JSON 文件，成功则返回 true，否则返回 false
bool writeJSON(const QJsonDocument& doc,
    const QString& path,
    const QStringConverter::Encoding& encoding = QStringConverter::Utf8,
    const bool& useConfigFolder = true);
