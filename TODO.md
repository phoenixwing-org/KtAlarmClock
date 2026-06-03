# KtAlarmClock 待办

## 背景问题

1. **计时**：屏幕休眠 / 合盖时 QML `Timer` 暂停，恢复后计时不准（工作计时、休息计时均受影响）。
2. **多屏锁屏**：HDMI 插拔、分辨率变化、其他窗口抢前台，可能导致锁屏失效（仅硬编码 screen 0/1）。
3. **解锁按钮**：可被拖拽元素遮挡；副屏 `MyOver1` 布局有 bug。

---

## 阶段 1：墙钟计时 — 代码完成，待验证

**目标**：用「开始时间 + 时长」代替「每秒 counter--」，休眠恢复后按真实经过时间判断。

### 实现说明

- **工作 / 休息计时**（`MyClock.qml`）：记录 `phaseStartMs` + `phaseDurationSec`，每秒及系统恢复时用 `Date.now()` 重算 `remaining`。
- **强制锁定期**（`MyOver0.js`）：记录 `forceEndMs`，不再随 tick 递减 `counterForce`。
- **恢复触发点**：`Qt.application.stateChanged` → `ApplicationActive`；锁屏窗口 `visible` 变化；锁屏内 1s 定时器。

### 任务

- [x] `MyClock.qml`：记录 `phaseStartMs` / `phaseDurationSec`，`syncFromWallClock()` 重算剩余时间
- [x] `MyClock.qml`：`Timer` 仅作 UI 刷新，暂停时保存剩余秒数
- [x] `KtAlarmClockDlg.qml`：监听 `Qt.application.stateChanged`，恢复时校准主计时
- [x] `MyOver0.qml` / `MyOver0.js`：`TimeForce` 强制锁定期改用 `forceEndMs` 墙钟
- [x] `MyOver0.qml`：锁屏可见时定时校准 + 应用恢复时立即校准
- [ ] 手动测试：工作计时中休眠、锁屏中休眠、TimeForce 强制期
- [ ] （可选）注册表持久化锁屏会话，进程重启后继续计时

### 涉及文件

- `KtAlarmClockUI/MyClock.qml`
- `KtAlarmClockUI/KtAlarmClockDlg.qml`
- `KtAlarmClockUI/MyOver0.qml`
- `KtAlarmClockUI/MyOver0.js`

---

## 阶段 2：解锁体验 — 未开始

**目标**：解锁入口始终可用，每块屏都能操作。

- [ ] 锁屏期间禁用 `KtMouseAreaMove`，或固定咖啡图 / 计时器位置
- [ ] 解锁按钮锚定屏幕右下角，`z` 值最高
- [ ] 修复 `MyOver1.qml` 中引用不存在 `footer` 的布局 bug
- [ ] 副屏显示完整解锁区（或固定位置解锁按钮）
- [ ] 锁屏期间每 3～5 秒 `raise()` / `requestActivate()`

### 涉及文件

- `KtAlarmClockUI/MyOver0.qml`
- `KtAlarmClockUI/MyOver1.qml`
- `KtAlarmClockUI/MyOver0.js`
- `KtAlarmClockUI/KtWindowOver.qml`

---

## 阶段 3：多屏自愈 — 未开始

**目标**：屏幕数量 / 分辨率变化后，锁屏仍覆盖所有显示器。

- [ ] 新建 `LockScreenManager`（JS 或 C++），按 `Qt.application.screens` 动态创建 overlay
- [ ] 监听 `screenAdded` / `screenRemoved`（或 10 秒 watchdog）
- [ ] `reconcileScreens()`：增删窗口、更新几何、全部置顶
- [ ] 替换 `MyOver0.js` 中硬编码 `screenId: 1` 的 `MyOver1` 方案

### 涉及文件

- 新建 `KtAlarmClockUI/LockScreenManager.js`（或 C++ helper）
- `KtAlarmClockUI/MyOver0.js`
- `KtAlarmClockUI/KtAlarmClock.js`

---

## 验收清单（阶段 1）

| 场景 | 期望 |
| --- | --- |
| 工作计时 15 分钟，中途休眠 5 分钟 | 醒来剩余约 10 分钟或直接锁屏 |
| 锁屏 10 分钟，TimeForce 2 分钟，休眠 8 分钟后醒来 | 强制期已过，约 2 分钟后可解锁 |
| 用户手动暂停 | 暂停期间不计时，恢复后继续 |
| 快进 / 后退 60 秒 | 剩余时间正确增减 |
