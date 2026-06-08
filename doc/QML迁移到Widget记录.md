# QML 迁移到 Widget 记录

> **备忘**：迁移工作已结束（2026-06）。  
> 主浮窗、托盘、设置页、业务逻辑、锁屏均为 C++ QWidget；QML/JS 已删除。验收项已全部通过（含 VDI 等同场置顶回顶测试）。  
> 后续需求在 QWidget 架构上迭代即可，**不再单独维护迁移待办**。

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

### 迁移与清理

- [x] `KtMainClockWidget` — 胶囊样式、拖动、墙钟 tick、`clock_out` 信号
- [x] `KtAlarmClockTray` — `QSystemTrayIcon` + 鼠标位置 popup
- [x] `KtAlarmClockController` — `run_command` / `clock_start` / `clock_timeout`
- [x] `KtAlarmClockSettingWindow` — Qt Designer `.ui` + `KtDurationEdit`/slider 绑定
- [x] `KtAlarmClockCmd::build` — 不再加载 QML 主窗
- [x] 删除 QML/JS 源文件
- [x] `KtAlarmClock.qrc` 仅保留 SVG 与 `lock-screen.qss`
- [x] 更新 `doc/` 文档（含 [多屏锁屏遮罩.md](./多屏锁屏遮罩.md)、[高分辨率适配.md](./高分辨率适配.md)）

### 迁移后增强（QWidget 路径）

- [x] 设置页 **默认** / **保存**；`set_param` → `update_dialog` 从 parameter 恢复
- [x] Next 按界面时长重计并 `registerWrite`
- [x] 多屏遮罩热插拔 `reconcile_screens()`（`screenAdded` / `screenRemoved`）
- [x] 主浮窗跨屏拖动与四边限制；右键菜单独立 `QMenu` + 宽度
- [x] 高 DPI：`app.manifest`（PerMonitorV2）+ `apply_layout_metrics`
- [x] 设置窗 `KtDurationEdit`（m:ss + `10s`）；`QPointer` 单例设置窗；参数 `sync` / `refresh_setting_ui`
- [x] 弹出界面跟随鼠标屏：`KtScreenUtil`

---

## 验收（已全部完成）

| 项 | 状态 | 备注 |
| --- | --- | --- |
| 托盘 / 主浮窗右键菜单在鼠标位置弹出 | ✅ | 已验证 |
| 工作 / 休息 / 暂停 / 快进后退 | ✅ | 与预期一致 |
| 设置页打开从 parameter 恢复；默认 / 保存 | ✅ | |
| 休眠唤醒墙钟校准 | ✅ | 未见异常 |
| 多屏遮罩与 HDMI 插拔 | ✅ | 见 [多屏锁屏遮罩.md](./多屏锁屏遮罩.md) |
| 高分辨率 / 多 DPI 设置窗与主浮窗 | ✅ | 见 [高分辨率适配.md](./高分辨率适配.md) |
| **与其他全屏置顶程序抢前台** | ✅ | VDI 等同场：他程序置顶后数秒内锁屏可回顶，效果 OK |

**目前未发现新问题。**

---

## 结论

- 迁移范围已全部落地，无未关闭的迁移项。
- 产品待办见 [TODO.md](./TODO.md)。

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
| `[LockScreen]` | 多屏 reconcile、遮罩增删 |
