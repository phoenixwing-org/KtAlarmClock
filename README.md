# KtAlarmClock

Qt 5 护眼闹钟：工作倒计时、全屏锁屏休息、多屏覆盖、托盘常驻。支持墙钟计时（休眠/合盖后仍准确）。

开源主页：[https://gitee.com/PhoenixWing321/KtAlarmClock](https://gitee.com/PhoenixWing321/KtAlarmClock)

---

## 快速开始

1. 配置环境变量 `ROOT_DIR`（输出根路径，如 `E:/XyRoot`）。
2. 编译 `KtAlarmClockUI` → `KtAlarmClock`（见下方构建说明）。
3. 运行 `KtAlarmClock.exe`，与 `KtAlarmClockUI.dll` 同目录。
4. 托盘或设置页调整 **工作 / 休息 / 强制锁定** 时长，点击播放开始。

---

## 软件架构

| 模块 | 路径 | 说明 |
| --- | --- | --- |
| 可执行入口 | `KtAlarmClock/` | `main.cpp`，加载 UI 插件 |
| UI 插件 | `KtAlarmClockUI/` | QWidget 界面、计时与锁屏逻辑（打进 dll） |
| 构建 | `CMakeLists.txt`、`common.cmake` | CMake 3.25+；产物默认 `${ROOT_DIR}/kt/viewer/debug` 与 `.../bin` |

技术栈：Qt 5 Widgets，C++20。QML/JS 主流程已迁移完成，现为纯 QWidget 实现（见 [doc/QML迁移到Widget记录.md](doc/QML迁移到Widget记录.md)）。

---

## 构建

### 环境要求

- CMake 3.25+
- Qt 5（Core、Gui、Widgets 等）
- MSVC / GCC / Clang（C++20）

### 环境变量

| 变量 | 说明 |
| --- | --- |
| `ROOT_DIR` | 工程输出根路径，例如 `E:/XyRoot`（Windows 必设；macOS 可选，未设则输出至 `build/` 内） |
| `ROOT_DIR_3rdParty` | 第三方库根路径（`common.cmake` 预留，可选） |

### 编译

项目使用 `CMakePresets.json` 管理跨平台构建预设。根据操作系统选择对应 preset：

**macOS：**

```bash
# Qt5 通过 Homebrew 安装：brew install qt@5
cmake --preset macos-debug
cmake --build --preset macos-debug
```

**Windows：**

```bash
cmake --preset win-debug
cmake --build --preset win-debug
```

> **平台说明**：`KtAlarmClock` 在 Windows 上通过 `LockWorkStation` API 锁屏；macOS 下通过全屏遮罩覆盖实现同等效果。两个平台均完整编译。

**注意**：图标与 `lock-screen.qss` 在 `KtAlarmClock.qrc` 中，改资源后须重新编译 `KtAlarmClockUI.dll`。

### 输出目录

| 类型 | Debug | Release |
| --- | --- | --- |
| 可执行文件 | `${ROOT_DIR}/kt/viewer/debug/KtAlarmClock.exe` | `.../bin/KtAlarmClock.exe` |
| 动态库 | `.../debug/KtAlarmClockUI.dll` | `.../bin/KtAlarmClockUI.dll` |

---

## 使用说明

1. **工作计时**：主浮窗显示倒计时；可拖动（左键），右键打开菜单。
2. **休息 / 锁屏**：到点或点「立刻休息」进入全屏；强制期内不可解锁。
3. **暂停**：仅工作计时可暂停；锁屏无暂停，始终按墙钟走。
4. **设置**：托盘 → 显示设置界面（标题含版本号）；底部可播放/暂停、快进/后退 60 秒、Next。
5. **单实例**：重复启动 exe 会提示并激活已有托盘进程。

更详细的计时与休眠行为见 **[doc/ 文档目录](doc/README.md)**。

---

## 文档

| 文档 | 说明 |
| --- | --- |
| **[doc/README.md](doc/README.md)** | 文档总索引 |
| [doc/计时系统说明.md](doc/计时系统说明.md) | 墙钟模型、工作/休息/暂停 |
| [doc/计时与休眠.md](doc/计时与休眠.md) | 合盖/休眠场景与**手动测试步骤** |
| [doc/计时相关文件索引.md](doc/计时相关文件索引.md) | C++ 源码与函数对照 |
| [doc/多屏锁屏遮罩.md](doc/多屏锁屏遮罩.md) | 多显示器遮罩与热插拔 reconcile |
| [doc/高分辨率适配.md](doc/高分辨率适配.md) | 高 DPI 适配：manifest、.ui 与 C++ |
| [doc/TODO.md](doc/TODO.md) | 产品待办（换肤、UI 美化） |
| [doc/QML迁移到Widget记录.md](doc/QML迁移到Widget记录.md) | QML→Widget 迁移备忘 |

---

## 开发约定

- 新建 C++ 遵循 `.cursor/skills/cxx-code-style`（`class_prefix: Kt`）
- 用户操作统一经 `KtAlarmClockController::dispatch_user_action(int actionId)`
- Debug 构建保留控制台黑窗口；Release 为 `WIN32` 纯 GUI
- 调试日志：Debug 构建默认 `set_debug_locate(true)`，前缀见 [QML迁移到Widget记录.md](doc/QML迁移到Widget记录.md)

---

## 待办（概要）

- 产品向：见 [doc/TODO.md](doc/TODO.md)
- 计时 / 休眠验收：见 [doc/计时与休眠.md](doc/计时与休眠.md#验收状态)

---

## 参与贡献

kevin、Jane
