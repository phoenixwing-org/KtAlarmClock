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

## 阶段 2：解锁体验 — 代码完成，待验证

**目标**：解锁入口始终可用，每块屏都能操作。

- [x] 锁屏期间移除 `KtMouseAreaMove`，咖啡图 / 计时器固定居中
- [x] 解锁区锚定屏幕右下角，`z: 100`
- [x] 修复 `MyOver1.qml` 中引用不存在 `footer` 的布局 bug
- [x] 副屏固定右下角解锁按钮 + 强制期倒计时显示
- [x] 锁屏期间每 5 秒 `raise()` / `requestActivate()`

### 涉及文件

- `KtAlarmClockUI/MyOver0.qml`
- `KtAlarmClockUI/MyOver1.qml`
- `KtAlarmClockUI/MyOver0.js`
- `KtAlarmClockUI/KtWindowOver.qml`
- `KtAlarmClockUI/KtWindowBase.qml`

---

## 阶段 3：多屏自愈 — 代码完成，待验证

**目标**：屏幕数量 / 分辨率变化后，锁屏仍覆盖所有显示器。

- [x] 新建 `LockScreenManager.js`，按 `Qt.application.screens` 动态创建副屏 overlay
- [x] 10 秒 watchdog + 应用恢复时 `reconcileScreens()`
- [x] `reconcileScreens()`：增删副屏窗口、更新几何、全部置顶
- [x] 替换 `MyOver0.js` 中硬编码 `Loader + MyOver1 screenId:1` 方案

### 涉及文件

- `KtAlarmClockUI/LockScreenManager.js`
- `KtAlarmClockUI/MyOver0.js`
- `KtAlarmClockUI/MyOver0.qml`

---

## 验收清单

### 阶段 1

| 场景 | 期望 |
| --- | --- |
| 工作计时 15 分钟，中途休眠 5 分钟 | 醒来剩余约 10 分钟或直接锁屏 |
| 锁屏 10 分钟，TimeForce 2 分钟，休眠 8 分钟后醒来 | 强制期已过，约 2 分钟后可解锁 |
| 用户手动暂停 | 暂停期间不计时，恢复后继续 |
| 快进 / 后退 60 秒 | 剩余时间正确增减 |

### 阶段 2 / 3

| 场景 | 期望 |
| --- | --- |
| 锁屏时拖拽 | 咖啡图 / 计时器不可移动，解锁按钮始终可见 |
| 双屏 | 每块屏右下角均有解锁按钮 |
| 拔掉副屏 HDMI | 10 秒内副屏 overlay 自动销毁，主屏锁屏正常 |
| 插入副屏 HDMI | 10 秒内新增副屏 overlay |
| 其他窗口抢前台 | 5 秒内锁屏窗口重新置顶 |
