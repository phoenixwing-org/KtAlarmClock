# KtAlarmClock 待办

> 构建与使用见 [../README.md](../README.md)  
> 计时设计见 [计时系统说明.md](./计时系统说明.md) · 测试步骤见 [计时与休眠.md](./计时与休眠.md)

---

## 背景问题

1. **计时**：屏幕休眠 / 合盖时 UI 定时器暂停，恢复后计时不准 → 已用墙钟方案解决（待验收）。
2. **多屏锁屏**：HDMI 插拔、分辨率变化、抢前台 → 已用 `KtLockScreenManager`（待验收）。
3. **解锁体验**：解锁区位置、副屏布局 → 已调整（待验收）。

---

## 阶段 1：墙钟计时 — 代码完成，待验证

**目标**：「起点 + 时长」+ 系统时钟，休眠恢复后按真实时间判断。

**文档**：[计时系统说明.md](./计时系统说明.md)、[计时与休眠.md](./计时与休眠.md)

### 实现要点

- **工作 / 休息**（`KtWallClockEngine`）：`phaseStartMs_` + `phaseDurationSec_`
- **强制期**（`KtLockScreenManager`）：`forceEndMs_` 绝对截止时间
- **暂停**：清 `phaseStartMs_`、冻结 `counter_`；继续时新设起点
- **校准**：`ApplicationActive`、`KtLockScreenManager` 1s tick、锁屏显示/唤醒

### 任务

- [x] `KtWallClockEngine` 墙钟与 `pause` / `start`
- [x] `KtMainClockWidget` + `ApplicationActive` 校准
- [x] `KtLockScreenManager` 休息钟 + `forceEndMs_`
- [x] 播放/暂停：`run_command(ActionPlayPause)` + 状态判断
- [ ] **手动测试**（步骤见 [计时与休眠.md § 手动验收步骤](./计时与休眠.md#手动验收步骤)）
- [ ] （可选）注册表持久化锁屏会话

### 涉及文件

`KtWallClockEngine` · `KtMainClockWidget` · `KtAlarmClockController` · `KtLockScreenManager`

---

## 阶段 2：解锁体验 — 代码完成，待验证

**目标**：解锁入口可用；茶杯+浮动计时可拖；强制期无中央大倒计时。

- [x] 解锁区左上角（margin 20）
- [x] 主屏茶杯组可拖动
- [x] 副屏左上角解锁
- [x] 解锁算式：仅可输入 0–9；回车/空格及其他非数字键递交，不写入文本框
- [x] 锁屏置顶：失焦静默 `raise_all()`；去掉周期 `requestActivate`（修复 Win10/11 闪屏）

### 涉及文件

`KtLockScreenPrimaryWidget` · `KtLockScreenSecondaryWidget` · `lock-screen.qss`

---

## 阶段 3：多屏自愈 — 代码完成，待验证

**目标**：插拔显示器后锁屏仍覆盖所有屏，且不多不少、不叠窗。

**策略文档**：[多屏锁屏遮罩.md](./多屏锁屏遮罩.md)

- [x] `reconcile_screens()`：按屏数增删副屏遮罩（多了 hide+销毁，少了新建）
- [x] `screenAdded` / `screenRemoved` 立即 reconcile
- [x] 10s watchdog + 唤醒时 reconcile（兜底）
- [x] 热插拔刷新 `primaryScreenId_`；无效 `screenId` 时 hide
- [x] 每块屏 `windowHandle()->setScreen()` 绑定

### 涉及文件

`KtLockScreenManager` · `KtLockScreenPrimaryWidget` · `KtLockScreenSecondaryWidget`

---

## 验收清单

### 阶段 1 — 计时与休眠

| # | 场景 | 期望 | 详细步骤 |
| --- | --- | --- | --- |
| 1 | 工作 15 分钟，中途休眠 5 分钟 | 醒来少约 5 分钟或进锁屏 | [计时与休眠 §2](./计时与休眠.md#场景-2工作中休眠) |
| 2 | 暂停后休眠 2 分钟 | 醒来剩余与暂停时相同 | [计时与休眠 §1](./计时与休眠.md#场景-1暂停后休眠) |
| 3 | 锁屏 + TimeForce，休眠 | 强制期/休息钟对齐墙钟 | [计时与休眠 §3](./计时与休眠.md#场景-3锁屏中休眠) |
| 4 | 点暂停 / 再播放 | 停住后从剩余继续 | [计时与休眠 §4](./计时与休眠.md#4-暂停按钮非休眠) |
| 5 | 快进 / 后退 60 秒 | 剩余时间 ±60 | 设置页底部按钮 |
| 6 | 主浮窗拖动 | 跟手、不拖出屏幕 | `KtMainClockWidget` |

### 阶段 2 / 3 — 锁屏与多屏

| 场景 | 期望 |
| --- | --- |
| 锁屏时拖茶杯组 | 可在屏内拖动；解锁钮在左上角 |
| 双屏 | 副屏有 overlay + 解锁 |
| 拔 HDMI | **立刻** 副屏 overlay 消失（`screenRemoved`）；日志 `secondaryCount` 减少 |
| 插 HDMI | **立刻** 新增副屏 overlay（`screenAdded`） |
| 抢前台 | 失焦后静默回顶；被挡时切回应用应恢复 |

---

## 产品待办

- [ ] 换肤
- [ ] UI 美化
- [x] 设置页按钮后持久化参数（`registerWrite`）与 Next 按界面时长重计
