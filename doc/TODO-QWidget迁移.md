# QWidget 迁移记录

> 主浮窗、托盘、设置页、业务逻辑、锁屏已全部迁到 C++ QWidget；QML/JS 已删除。

---

## 迁移对照

| 模块 | 原实现（已删除） | 现实现 | 状态 |
|------|------------------|--------|------|
| 锁屏 | `MyOver0.qml` + JS | `KtLockScreenManager` + QWidget | ✅ |
| 墙钟引擎 | `MyClock.qml` | `KtWallClockEngine` | ✅ |
| 主浮窗 | `MyClock.qml` + `KtAlarmClockDlg.qml` | `KtMainClockWidget` | ✅ |
| 业务逻辑 | `KtAlarmClock.js` | `KtAlarmClockController` | ✅ |
| 托盘 | `KtAlarmClockSystemTrayIcon.qml` | `KtAlarmClockTray` | ✅ |
| 右键菜单 | `MyWindowMenu.qml` | `QMenu::popup(cursorPos)` | ✅ |
| 设置页 | `KtAlarmClockSettingDlg.qml` | `KtAlarmClockSettingWindow` + `.ui` | ✅ |
| 主题/状态 | `KtAlarmTheme.qml` | Controller 成员 `workStep_` / `loop_` | ✅ |

---

## 已完成项

- [x] `KtMainClockWidget` — 胶囊样式、拖动、墙钟 tick、`clock_out` 信号
- [x] `KtAlarmClockTray` — `QSystemTrayIcon` + 鼠标位置 popup
- [x] `KtAlarmClockController` — `run_command` / `clock_start` / `clock_timeout`
- [x] `KtAlarmClockSettingWindow` — Qt Designer `.ui` + spinBox/slider 绑定
- [x] `KtAlarmClockCmd::build` — 不再加载 QML 主窗
- [x] 删除 QML/JS 源文件
- [x] `KtAlarmClock.qrc` 仅保留 SVG 与 `lock-screen.qss`
- [x] 更新 `doc/` 文档索引

---

## 待验收

- [ ] 托盘/主浮窗右键菜单在鼠标位置弹出
- [ ] 工作/休息/暂停/快进后退与迁移前行为一致
- [x] 设置页 Next 按界面时长重计并保存参数
- [ ] 休眠唤醒后墙钟校准（`ApplicationActive`）
- [x] 多屏遮罩：每屏独立 `KtLockScreenSecondaryWidget`
- [x] 多屏插拔：`reconcile_screens()` 增删遮罩（策略见 [多屏锁屏遮罩.md](./多屏锁屏遮罩.md)）

---

## 代码规范

新建 C++ 文件遵循：`.cursor/skills/cxx-code-style`（`class_prefix: Kt`，UTF-8）。

---

## 调试日志前缀

| 前缀 | 含义 |
|------|------|
| `[Controller]` | 命令、状态切换、计时到期 |
| `[Tray]` | 托盘激活、菜单坐标 |
| `[PopMenu]` | 主浮窗右键菜单坐标 |
| `[MainClock]` | 拖动、显示/隐藏 |
| `[Setting]` | 设置窗打开/关闭、参数读写 |
