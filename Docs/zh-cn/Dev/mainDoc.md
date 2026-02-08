# 主开发文档

> [!ATTENTION]
> Python版本（目录`PythonVersion`）的客户端已经完全停止支持！且不提供任何开发文档，所有文档都指C++的版本。
> 
> 原因：Python不够底层，且Python版本太多代码都是AI写的，也确实该彻底重构了。而且C++更方便我装逼（doge

> [!NOTE]
> 建议结合搜索引擎和视频平台来学习其中的内容，如软件、插件的安装、环境配置等。

- 开发工具：`Microsoft Visual Studio Community 2026`
- Qt及其编译器版本：`6.5.3_msvc2019_64`
- Windows SDK版本：`Windows 10.0 SDK (最新)`
- 平台工具集：`v145`
- C++语言标准：`预览 - ISO C++23 标准 (/std:c++23preview)`
- 第三方库管理工具：`Microsoft vcpkg package management program version 2025-10-16-71538f2694db93da4668782d094768ba74c45991`
- 代码托管平台：`Github`
- 文档基于：`Docsify`
- 适用的操作系统：`Microsoft Windows 10 ~ 11`，更低版本不保证能用
- 开发框架：`Qt`，一个跨平台应用开发框架（framework），用C++编写的大型类库
- 作者主页：`https://github.com/fuxianhu/`
- 仓库地址：`https://github.com/fuxianhu/VoidCrawler`
- 克隆URL：`https://github.com/fuxianhu/VoidCrawler.git`
- 开始开发时间：$2025$ 年 $10$ 月左右

> [!TIP]
> 如果您想要对项目贡献或阅读源码，请务必熟悉 `Qt`。
> 
> 本项目不打算使用 QML，而是用 Qt Widght。
> 
> 本项目会更多地使用代码化 UI 设计，可视化 UI 设计会减少使用，这样方便以后让用户对 UI 进行深度自定义。

## 自行构建项目

- 克隆仓库，注意项目用了 Github LFS，使用 `git lfs pull` 等命令将大文件一并克隆下来，仓库地址等信息见文档开头
- 配置 Qt 环境（版本见文档开头），[官网链接](https://www.qt.io/zh-cn/)
- 安装 Microsoft Visual Studio，[官网链接](https://visualstudio.microsoft.com/zh-hans/)
- 打开 Microsoft Visual Studio
- 安装 Qt VS Tools 扩展。（在上方的标题栏中的 扩展>管理扩展，安装它并重新启动 Microsoft Visual Studio 以生效）
- 尝试生成解决方案

如果尝试导入第三方库时报错，那么你需要安装和配置 vcpkg:
- 在终端中打开 `\VoidCrawler` 目录（使用 `cd` 命令）
- 安装 vcpkg，确保能正常编译
- 运行 `vcpkg install` 命令安装依赖库

> [!NOTE]
> 除特殊情况，所有文件都要用 Unicode 带签名（万国码，统一码） UTF-8 with BOM 编码！优先用 CRLF，减少因编码导致的乱码问题和因换行方式导致的换行问题。
> 
> 对于其他非文本文件（如资源文件），其格式暂不做任何限制。

## 发布软件

1. 安装 Powershell，作者使用 `7.5.4` 版本
2. 清理解决方案，并使用 `Releses x64` 生成解决方案，若使用 `Debug` 或 `x86` 等则会导致卡顿或无法编译等问题
3. 在 `VoidCrawler` 目录下，使用 PowerShell 运行 `release.ps1`
4. 在 `VoidCrawler\x64\Releses\` 中，便是我们的软件了，其中有一个压缩包，你可以把它传给任何人

## 提交 PR

欢迎你为项目做出任何贡献！
