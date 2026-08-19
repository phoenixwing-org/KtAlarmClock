# KtAlarmClock 文档

本目录描述当前全 Rust 实现的产品语义、代码边界与验收方法。旧 Qt/C++ 源码已经从 `rust` 分支删除；迁移前基线可从 Git commit `16c00a4` 查看。

## 建议阅读顺序

1. [计时系统说明.md](./计时系统说明.md)：状态、时钟和 MVC 边界；
2. [计时与休眠.md](./计时与休眠.md)：工作累加/休眠冻结、休息墙钟相减的规格与验收；
3. [多屏锁屏遮罩.md](./多屏锁屏遮罩.md)：初次覆盖、热插拔、窗口生命周期；
4. [资源测试.md](./资源测试.md)：独立 Release 进程的体积、内存和 CPU；
5. [Windows与CI验证.md](./Windows与CI验证.md)：Windows 交叉检查、CI 选择和实机边界；
6. [计时相关文件索引.md](./计时相关文件索引.md)：修改代码时定位 crate 与函数。
7. [计时逻辑对照审计.md](./计时逻辑对照审计.md)：Rust 与 QWidget 的计时语义差异及修正边界。

## 文档索引

| 文档 | 内容 |
| --- | --- |
| [Clock去Qt与Rust重写可行性分析.md](./Clock去Qt与Rust重写可行性分析.md) | 旧项目调查、UI 选型、全 Rust 决策、阶段实测和验证边界 |
| [计时系统说明.md](./计时系统说明.md) | domain 状态机、时间源、Controller/Effect |
| [计时与休眠.md](./计时与休眠.md) | 系统休眠语义、自动测试与人工真机验收 |
| [多屏锁屏遮罩.md](./多屏锁屏遮罩.md) | 每屏一个 Slint 窗口、原生置顶、拓扑 reconcile |
| [高分辨率适配.md](./高分辨率适配.md) | winit/Slint 物理像素、缩放和混合 DPI |
| [资源测试.md](./资源测试.md) | Release 子进程指标、JSON 报告、可选 CI 门槛 |
| [Windows与CI验证.md](./Windows与CI验证.md) | macOS 可做的 Windows 检查、GitHub/Gitee CI 对比与发布验收 |
| [计时相关文件索引.md](./计时相关文件索引.md) | 当前 Rust 文件与职责索引 |
| [计时逻辑对照审计.md](./计时逻辑对照审计.md) | Rust / QWidget 计时逻辑对照与 Windows 休眠排查 |
| [TODO.md](./TODO.md) | 尚未实现的产品体验项，不混入迁移任务 |

## 架构一览

```text
Slint View（app/ui）
        ↓ callback / property
Controller（clock-app）
        ↓ event / effect
ClockModel（clock-domain）
        ↓ platform effect
Windows/macOS adapters（clock-platform）
```

平台适配器不包含按钮、布局或业务状态，只处理原生系统能力；Windows 和 macOS 共用同一份 `.slint` UI。
