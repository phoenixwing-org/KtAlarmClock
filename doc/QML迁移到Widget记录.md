# QML 迁移到 Widget 记录

> 历史记录。这个文件描述的是早期从 QML/JS 迁移到 QWidget 的阶段，当前项目方向已经再次调整为 **C++ 业务核心 + QML UI**。

当前状态请以这些文件为准：

- [../README.md](../README.md)
- [README.md](./README.md)
- [计时相关文件索引.md](./计时相关文件索引.md)
- [多屏锁屏遮罩.md](./多屏锁屏遮罩.md)

## 当前结论

- 主浮窗：QML，入口为 `res/qml/Main.qml` 和 `ClockBubble.qml`。
- 设置界面：QML，入口为 `SettingsPanel.qml`。
- 锁屏界面：QML，入口为 `LockPrimary.qml` / `LockSecondary.qml`。
- 托盘：继续使用 `QSystemTrayIcon`，但隔离在 `KtTrayService`。
- 精确计时：继续由 C++ QPC/monotonic countdown 负责。

## 保留价值

这份文档名暂时保留，是为了提醒维护者：如果在旧提交或历史讨论里看到 QWidget 迁移路径，那已经不是当前目标架构。后续新增说明应写入当前文档，而不是继续扩展本历史记录。
