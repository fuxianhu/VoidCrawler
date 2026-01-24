# 配置文件格式 Configuration File Format

## 前言

配置文件统一存储在 `.\config\` 目录下，文件结构：

```
config
|___ ItemConfiguration.json
|___ main.json
|___ category.json
```

## 主配置文件 main.json

配置文件位置：`.\config\main.json`

以下是其默认值与注释

```json
{
  "agreeTerms": false, // 是否同意协议，正式版默认应为 false, 开发版本为 true
  "clientLock": { // 客户端童锁（使用哈希函数加密），注意：只防不能够熟悉使用客户端的人
    "disableClient": false, // 是否禁用客户端
    "maximumAttempts": 3, // 最大重试次数
    "password": "", // 密码（哈希后）
    "salted": "" // 盐（哈希加盐算法）
  },
  "configVersion": "v.0.0.1.5", // 配置文件对应的客户端版本
  "enableMultipleApplications": false, // 启用客户端多开
  "language": "zh-cn", // 语言
  "log": { // 日志配置
    "level": "debug", // 最低级别
    "logFolder": "log", // 日志存储文件夹
    "logName": "VoidCrawler", // 日志文件名（logName_xxx.log）
    "maxFileSize": 1048576, // 最大文件大小
    "maxFiles": 5 // 最大文件数量，多出的文件自动删除
  },
  "userName": "Hacker", // 用户名
  "font": { // 字体
    "global": { // 全局
      "family": "Microsoft YaHei", // 字体
      "pointSize": -1, // 大小，-1为不设置，看更低级别的
      "weight": -1 // -1为不设置，看更低级别的
    },
    "title": {
      "family": "Holland",
      "pointSize": 13,
      "weight": 700
    },
    "item": {
      "family": "Gabriola",
      "pointSize": 15,
      "weight": -1
    },
    "text": {
      "family": "楷体",
      "pointSize": -1,
      "weight": -1
    }
  },
  "animationSpeed": 4000 // 动画速度，数值越大越慢（相反）
}
```

## 分类配置文件 category.json

配置文件位置：`.\config\category.json`

此配置文件只会在加载项目列表时读取，大多数条目不会修改。

以下是其默认值与注释

```json
[
  {
    "hide": false, // 是否隐藏，某些情况不可用
    "id": "VoidCrawler", // 窗口名称
    "items": [ // 项目（功能）列表
      {
        "description": "Run cmd.exe. ", // 描述
        "help": "Conveniently run cmd.exe program. ", // 帮助信息，相较于描述更详细
        "icons": null, // 图标，null表示无
        "id": "cmd", // 唯一表示符，不可修改，否则会导致 Bug
        "type": 0 // 项目（功能）类型，0表示普通按钮，1表示开关
      },
      {
        "description": "Run pwsh.exe. ",
        "help": "Conveniently run pwsh.exe (New version) program. ",
        "icons": null,
        "id": "NewPowershell",
        "type": 0
      },
      {
        "description": "Run powershell.exe. ",
        "help": "Conveniently run powershell.exe (Old version) program. ",
        "icons": null,
        "id": "OldPowershell",
        "type": 0
      },
      {
        "description": "Open Visual Studio Code program. This is a open source AI code editor. Official Website: https://code.visualstudio.com/",
        "help": "Open Visual Studio Code Program (A Code Editor).",
        "icons": null,
        "id": "VisualStudioCode",
        "type": 0
      },
      {
        "description": "Open Visual Studio program. This is a Windows OS code editor. Official Website: https://visualstudio.microsoft.com/",
        "help": "Open Visual Studio Program (A Code Editor).",
        "icons": null,
        "id": "VisualStudio",
        "type": 0
      },
      {
        "description": "Open Taskmgr program. ",
        "help": "Run Taskmgr.exe. ",
        "icons": null,
        "id": "WindowsTaskManger",
        "type": 0
      },
      {
        "description": "Open Microsoft Edge Browser Program (Run msedge.exe).",
        "help": "Open Edge Browser Program (Run msedge.exe).",
        "icons": null,
        "id": "EdgeBrowser",
        "type": 0
      },
      {
        "description": "Open Weixin Software (Run Weixin.exe).",
        "help": "Open Weixin Software",
        "icons": null,
        "id": "Weixin",
        "type": 0
      },
      {
        "description": "Open Calc Program (Run calc.exe).",
        "help": "run calc.exe . ",
        "icons": null,
        "id": "WindowsCalc",
        "type": 0
      },
      {
        "description": "Open user folder in explorer.exe. (%USERPROFILE%)",
        "help": "Open user folder in Explorer.",
        "icons": null,
        "id": "OpenUserFolder",
        "type": 0
      },
      {
        "description": "Change character set.",
        "help": "Run chcp <a number>, change character set.",
        "icons": null,
        "id": "chcp",
        "type": 0
      },
      {
        "description": "Switch",
        "help": "Switch",
        "icons": null,
        "id": "SwitchIMECtrlSpace",
        "switch": true,
        "type": 1
      },
      {
        "description": "Switch",
        "help": "Switch",
        "icons": null,
        "id": "SwitchIMEShift",
        "switch": false,
        "type": 1
      },
      {
        "description": "Switch",
        "help": "Switch",
        "icons": null,
        "id": "SwitchIMECtrl",
        "switch": false,
        "type": 1
      },
      {
        "description": "Audio Player",
        "help": "A Audio Player.",
        "icons": null,
        "id": "AudioPlayer",
        "type": 0
      },
      {
        "description": "Open Minecraft China Edition",
        "help": "Open Minecraft China Edition (fevergames://mygame/?gameId=1).",
        "icons": null,
        "id": "OpenMinecraftChinaEdition",
        "type": 0
      },
      {
        "description": "Open Bjd in Minecraft",
        "help": "Open Bjd in Minecraft China Edition.",
        "icons": null,
        "id": "OpenBjd",
        "type": 0
      },
      {
        "description": "Open Bjd Official Website",
        "help": "Open Bjd official website in your browser.",
        "icons": null,
        "id": "OpenBjdOfficialWebsite",
        "type": 0
      },
      {
        "description": "Exit Client. ",
        "help": "Exit VoidCrawler Client. ",
        "icons": null,
        "id": "Exit",
        "type": 0
      }
    ]
  }
]
```

## 项目配置文件 ItemConfiguration.json

配置文件位置：`.\config\ItemConfiguration.json`

以下是其默认值与注释

```json
{
  "cmd": {
    "CommandLineArguments": null, // 命令行参数
    "permission": null, // 权限，null表示继承客户端权限
    "workingDirectory": null // 工作目录
  },
  "NewPowershell": {
    "CommandLineArguments": null,
    "permission": null,
    "workingDirectory": null
  },
  "OldPowershell": {
    "CommandLineArguments": null,
    "permission": null,
    "workingDirectory": null
  },
  "VisualStudioCode": {
    "CommandLineArguments": null,
    "permission": null,
    "workingDirectory": null
  },
  "VisualStudio": {
    "CommandLineArguments": null,
    "permission": null,
    "workingDirectory": null
  },
  "WindowsTaskManger": {
    "CommandLineArguments": null,
    "permission": null
  },
  "EdgeBrowser": {
    "CommandLineArguments": null,
    "permission": null
  },
  "Weixin": {
    "CommandLineArguments": null,
    "permission": null
  },
  "WindowsCalc": {
    "CommandLineArguments": null
  },
  "OpenUserFolder": {
    "permission": null
  },
  "chcp": {
    "encoding": 65001 // 编码方式，如 65001 表示 UTF-8
  },
  "AudioPlayer": {
    "LoopPlayMode": 1, // 循环模式，0 表示不循环，1 表示单曲循环，2 表示歌单循环
    "audio": null, // 音频文件位置，可以是网址
    "position": 0, // 记录上次的播放位置
    "AudoSave": true // 是否自动保存配置
  }
}
```
