# KtAlarmClock（QWidget 分支）

轻量的工作/休息计时器。当前分支是 **Qt 5 + C++ + QWidget** 实现：工作时显示常驻桌面的计时胶囊，休息时为每块显示器创建全屏遮罩，强制休息期结束后通过算式解锁。程序只处理本机用户交互，不调用 Windows 工作站锁定 API，也不负责远程工作站锁屏。

开源主页：[https://gitee.com/phoenixwing/KtAlarmClock](https://gitee.com/phoenixwing/KtAlarmClock)

## 分支说明

仓库保留三条相互独立的实现路线，请在对应分支编译、测试和提交：

| 分支              | 实现                                   | 构建入口     |
| ----------------- | -------------------------------------- | ------------ |
| `qml`             | Qt 5 + QML/JavaScript 旧实现           | CMake / Qt 5 |
| `qwidget`（当前） | Qt 5 + C++ + QWidget                   | CMake / Qt 5 |
| `rust`            | Rust + Slint + winit software renderer | Cargo        |

本 README 只描述 `qwidget` 分支；Rust 迁移设计、资源采样和 Windows CI 说明请以 `rust` 分支的 README/docs 为准。

## 快速开始

1. 安装 CMake 3.25+、Qt 5（Core、Gui、Widgets）和 C++20 编译器。
2. 按操作系统选择 CMake preset 并编译。
3. 运行生成的 `KtAlarmClock`（Windows 下为 `KtAlarmClock.exe`）。

## 软件架构

| 模块       | 路径                             | 说明                                             |
| ---------- | -------------------------------- | ------------------------------------------------ |
| 可执行入口 | `KtAlarmClock/`                  | `main.cpp`，启动应用与插件                       |
| UI 与业务  | `KtAlarmClockUI/`                | QWidget 界面、计时、托盘、设置和多屏遮罩         |
| 构建       | `CMakeLists.txt`、KtRoot/cmake/SdkCore.cmake | CMake 3.25+；产物目录由 `ROOT_DIR`、`SDK_PREFIX` 和目标平台自动决定 |

QML/JavaScript 主流程已迁移完成；本分支后续只维护 QWidget 路径。详细迁移记录见 [doc/QML迁移到Widget记录.md](doc/QML迁移到Widget记录.md)。

## 构建

### 环境变量

| 变量                | 说明                                                                              |
| ------------------- | --------------------------------------------------------------------------------- |
| `ROOT_DIR`          | 工程输出根路径，例如 `E:/XyRoot`（Windows 建议设置；macOS 未设置时使用 `build/`） |
| `ROOT_DIR_3rdParty` | 第三方库根路径（可选）                                                            |

### macOS

```bash
cmake --preset macos-debug
cmake --build --preset macos-debug
```

### Windows

```bash
cmake --preset win-debug
cmake --build --preset win-debug
```

Debug 构建保留控制台，便于查看诊断日志；Release 构建使用纯 GUI 子系统。修改 `.ui`、QSS 或 `.qrc` 后，需要重新构建 `KtAlarmClockUI`。

## 使用约定

- 工作计时：主浮窗显示倒计时，可左键拖动，右键打开菜单。
- 休息遮罩：到点或选择“立刻休息”后覆盖所有当前显示器；首次创建应在同一事件批次完成，运行中持续检查显示器变化。
- 强制休息：强制期未结束时不能通过普通操作解锁；休息到点后允许算式解锁。
- 休眠：工作状态按 Qt 事件与墙钟校核；休息状态唤醒后按进入休息时间点补算，避免误判。
- 设置：通过托盘或右键菜单打开；工作、休息和强制时长按旧 Qt 输入规则校验。

## 测试与点检

基础构建与自动测试：

```bash
cmake --build --preset macos-debug
ctest --test-dir build/mac --output-on-failure
```

人工点检按 [doc/分支点检表.md](doc/分支点检表.md) 执行；计时、休眠和多屏专项步骤见：

- [doc/计时系统说明.md](doc/计时系统说明.md)
- [doc/计时与休眠.md](doc/计时与休眠.md)
- [doc/多屏锁屏遮罩.md](doc/多屏锁屏遮罩.md)
- [doc/高分辨率适配.md](doc/高分辨率适配.md)

Windows 的真实链接、休眠、自启动、双/三屏和混合 DPI 仍需在 Windows CI 或实机完成；macOS 不能替代 Windows GUI 行为验收。

macOS Debug 可执行文件位于 `build/mac/out/debug/KtAlarmClock`。

## 文档

文档总索引见 [doc/README.md](doc/README.md)。源码文件与函数对应关系见 [doc/计时相关文件索引.md](doc/计时相关文件索引.md)，未完成产品事项见 [doc/TODO.md](doc/TODO.md)。

## License

[Apache-2.0](LICENSE)
