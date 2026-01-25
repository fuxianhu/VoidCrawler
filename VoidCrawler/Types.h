#pragma once
#include <string>
#include <QString>
#include <type_traits>
#include <string_view>
#include <concepts>


enum Types
{
    TypeError = -1, // 类型错误，不用于表示哪种异常，而是表示类型无法识别
    Null = 0,
    Bool = 1,
    Integer = 2, // 方便起见，囊括 NBaseNumber
    Double = 3, // 方便起见，囊括 NBaseNumber
    String = 4,
    Color = 5,
    File = 6,
    Folder = 7,
    Font = 8,
    Link = 9,
    Dict = 10,
    Time = 11,
    Date = 12,
    DateTime = 13,
    Regedit = 14,
    Permission = 15,
    Task = 16,
    List = 17,
    Table = 18,
    Binary = 19, // 与2进制数不同，其可以用于存储更复杂的选项
    SingleChoice = 20, // 单选
    MultipleChoice = 21, // 多选
};

const char* ColorToString(Types c)
{
    switch (c)
    {
    case TypeError: return "TypeError";
    case Null: return "Null";
    case Bool: return "Bool";
    case Integer: return "Integer";
    case Double: return "Double";
    case String: return "String";
    case Color: return "Color";
    case File: return "File";
    case Folder: return "Folder";
    case Font: return "Font";
    case Link: return "Link";
    case Dict: return "Dict";
    case Time: return "Time";
    case DateTime: return "DateTime";
    case Regedit: return "Regedit";
    case Permission: return "Permission";
    case Task: return "Task";
    case List: return "List";
    case Table: return "Table";
    case Binary: return "Binary";
    case SingleChoice: return "SingleChoice";
    case MultipleChoice: return "MultipleChoice";
    default: return "TypeError";
    }
}

template<typename T>
Types StringToColor(const T& s)
{
    if (s == "TypeError") return TypeError;
    if (s == "Null") return Null;
    if (s == "Bool") return Bool;
    if (s == "Integer") return Integer;
    if (s == "Double") return Double;
    if (s == "String") return String;
    if (s == "Color") return Color;
    if (s == "File") return File;
    if (s == "Folder") return Folder;
    if (s == "Font") return Font;
    if (s == "Link") return Link;
    if (s == "Dict") return Dict;
    if (s == "Time") return Time;
    if (s == "Date") return Date;
    if (s == "DateTime") return DateTime;
    if (s == "Regedit") return Regedit;
    if (s == "Permission") return Permission;
    if (s == "Task") return Task;
    if (s == "List") return List;
    if (s == "Table") return Table;
    if (s == "Binary") return Binary;
    if (s == "SingleChoice") return SingleChoice;
    if (s == "MultipleChoice") return MultipleChoice;
    return TypeError;
}