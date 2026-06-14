# KtAlarmClock Docs

This directory contains design and maintenance notes for the current Qt/QML implementation.

## Recommended Reading

1. [计时系统说明.md](./计时系统说明.md) - countdown phases, forced break behavior, and sleep/wake timing notes.
2. [计时相关文件索引.md](./计时相关文件索引.md) - current source-file map.
3. [多屏锁屏遮罩.md](./多屏锁屏遮罩.md) - lock-screen multi-monitor orchestration.
4. [高分辨率适配.md](./高分辨率适配.md) - DPI notes. Some older Widget-specific notes are historical.
5. [QML迁移到Widget记录.md](./QML迁移到Widget记录.md) - historical record only; the current UI direction is QML again.

## Current Shape

```text
KtAlarmClock.exe
  -> KtAlarmClockCmd
     -> KtAlarmClockController
        -> KtClockRuntime
        -> KtAlarmClockViewModel
        -> KtAlarmClockQmlShell
        -> KtLockScreenManager
        -> KtTrayService
```

QML owns visible interface composition. C++ owns timing, settings, tray integration, and system-window orchestration.
