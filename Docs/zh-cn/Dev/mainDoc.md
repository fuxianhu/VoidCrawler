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
- 配置 Qt 环境（版本见文档开头），官网：`https://www.qt.io/zh-cn/`
- 安装 `Microsoft Visual Studio`，官网：`https://visualstudio.microsoft.com/zh-hans/`
- 打开 `Microsoft Visual Studio`
- 安装 `Qt VS Tools` 扩展。（在上方的标题栏中的 `扩展>管理扩展`，搜索 `Qt` 安装它并重新启动 `Microsoft Visual Studio` 以生效）
- 尝试生成解决方案

> [!NOTE]
> 大部分文件为 UTF-8 with BOM 编码