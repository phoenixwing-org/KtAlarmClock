# KtAlarmClock

## 介绍

KT 闹钟程序。

## 软件架构

- Qt 5，基于 Qt Quick / QML 界面
- 工程分为两个目标：
  - `KtAlarmClockUI`：QML 插件动态库，包含界面与业务逻辑
  - `KtAlarmClock`：可执行程序入口
- 构建系统：CMake 3.25+

## 构建

### 环境要求

- CMake 3.25 及以上
- Qt 5（Core、Qml、Quick、Multimedia）
- C++20 编译器（MSVC / GCC / Clang）

### 环境变量

构建前需设置以下环境变量：

| 变量 | 说明 |
| --- | --- |
| `ROOT_DIR` | 工程根路径，例如 `E:/XyRoot` |
| `ROOT_DIR_3rdParty` | 第三方库根路径（`common.cmake` 预留） |

### 编译步骤

子目录顺序即为依赖顺序：先编译 `KtAlarmClockUI`，再编译 `KtAlarmClock`。

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Debug
cmake --build . --config Release
```

### 输出目录

输出路径由 `common.cmake` 统一管理，默认基于 `${ROOT_DIR}/kt/viewer`：

| 类型 | Debug | Release |
| --- | --- | --- |
| 可执行文件 | `debug/KtAlarmClock.exe` | `bin/KtAlarmClock.exe` |
| 动态库 | `debug/KtAlarmClockUI.dll` | `bin/KtAlarmClockUI.dll` |
| 静态库 | `lib/` | `lib/` |

## 安装教程

1. 绿色软件，直接运行，需要 Qt 5 相关 DLL 支持。
2. 将 `KtAlarmClock.exe` 与 `KtAlarmClockUI.dll` 放在同一目录下。

## 使用说明

1. 设定总时间、休息时间、工作时间。
2. 到时机后会锁定屏幕几分钟，无法解锁。
3. 解锁结束后，可以继续操作电脑。
4. 可以用于 PPT 演讲的计时。

详细请查看 wiki 使用说明。

## 待完成工作

1. 换肤功能
2. UI 美化

## 开发说明

### QML 约定

- QML 对象的构造顺序：id → 属性声明 → 信号声明 → JavaScript 函数 → 对象属性 → 子对象 → 状态 → 状态切换
- 私有属性使用两个下划线开头：`__`

## 参与贡献

1. kevin
2. Jane
