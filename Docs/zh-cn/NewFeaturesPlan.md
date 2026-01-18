# 新功能计划 New Features Plan

## Auto Click 自动点击

允许有多个实例（List），对于每个实例有：

- 按键：`Left`, `Middle`, `Right`
- 时间单位：`MilliSecond`, `Second`, `Minute`, `Hour`, `Day`, `Week`, `Month`, `Year`

选项：
| 键 | 类型 | 默认值 | 描述 |
| --- | --- | --- | --- |
| 按键`Key` | 单选`Select` | 左键`Left` | 需要点击的鼠标按键 |
| 使用速度/间隔选项`SpeedOrInter` | 开关`Bool` | 开`True` | 使用速度选项还是间隔选项 |
| 速度`Speed` | 整数`Int` | $5$ | 每个单位点击多少次 |
| 速度单位`SpeedUnit` | 单选`Select` | 秒`Second` | 速度使用的时间单位 |
| 间隔`Inter` | 整数`Int` | $5$ | 每相隔多少个时间单位点击一次 |
| 间隔单位`InterUnit` | 单选`Select` | 毫秒`MilliSecond` | 间隔使用的时间单位 |
| 持续时间`Time` | 整数`Int` | $0$ | 持续点击多少个时间单位，$0$表示无穷 |
| 持续时间单位`TimeUnit` | 单选`Select` | 秒`Second` | 持续时间使用的时间单位 |
| 启用客户端内快捷键`EnableClientShortcutsKey` | 开关`Bool` | 关`False` | 启用/禁用客户端内快捷键 |
| 客户端内快捷键`ClientShortcutsKey` | 按键`Keys` | `F8` | 客户端内快捷键 |
| 启用全局热键`EnableGlobalHotkey` | 开关`Bool` | 关`False` | 启用/禁用全局热键 |
| 全局热键`GlobalHotkey` | 按键`Keys` | `F8` | 全局热键 |

## Auto Pause 自动按键
