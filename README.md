# KtAlarmClock

轻量的工作/休息计时器。主界面是一个常驻桌面的计时胶囊；休息时为每块显示器创建全屏遮罩，强制期结束后可通过算式提前解锁，休息到点后可直接解锁。

当前 `rust` 分支已经是 **全 Rust、无 Qt、无 QML、无 WebView** 实现：

- 一套 Slint UI 同时用于 Windows 和 macOS；
- winit 原生窗口后端 + software renderer；
- `windows` 与 `objc2` / `objc2-app-kit` 只负责薄平台适配，不维护两套 UI；
- 业务状态机、配置、单实例、休眠语义和资源采样都有自动测试。

## 行为约定

- 工作、暂停和打开设置时，胶囊始终显示并置于普通窗口上方；休息时胶囊隐藏，休息遮罩使用更高的原生窗口层级。
- 胶囊是随时间文字自动收缩的深灰色半圆角标签，时间文字沿用设置界面的亮蓝色；不放图标和按钮。左键拖动可移动，右键菜单提供开始/暂停、立刻休息、设置和退出。时间增减只放在设置窗口。
- 胶囊拖动或文字宽度变化后会自动限制在当前屏幕工作区内，不会跑到菜单栏、Dock 或 Windows 任务栏之外。
- 设置窗沿用旧版黑底蓝色主题与五个 SVG 动作按钮；三组时长均以 10 秒步进，并支持旧 Qt 的 `分:秒`、`Ns`（秒）和纯数字分钟输入。底部动作先提交设置再执行；窗口由内容决定固定尺寸，用户不能拉伸。
- macOS 胶囊使用透明、无系统阴影的原生窗口和 CALayer 圆角裁剪；Windows 使用 DPI 感知的圆角窗口区域和 `HWND_TOPMOST`。两端的内容仍由同一份 Slint UI 绘制。
- macOS 以辅助应用运行，不显示 Dock 图标，也不占用 `Command+Tab`；操作入口保留在胶囊右键菜单和菜单栏托盘。
- 工作计时使用单调时钟；系统真正挂起时冻结，唤醒后从冻结点继续。
- 休息倒计时和强制期使用墙钟绝对截止时间；系统休眠时间计入休息，唤醒后立即补算。
- 休息到点不会自动关闭遮罩，而是允许用户解锁。
- 显示与触发允许约 ±1 秒误差；只运行 1 秒定时器，不追求亚秒精度。
- 多屏目标是同一事件循环批次创建全部遮罩；运行中每秒检查一次显示器拓扑并补齐热插拔变化。

## 构建

要求：

- Rust 1.96 或更高版本；
- macOS：Xcode Command Line Tools；
- Windows：Visual Studio Build Tools（MSVC）与 Windows SDK。

```bash
cargo build --release
```

产物：

- macOS/Linux：`target/release/KtAlarmClock`
- Windows：`target/release/KtAlarmClock.exe`

程序运行时不需要 Qt、浏览器内核或额外 UI 动态库。Slint UI 和实际使用的 SVG 会在构建时编入程序。

开发运行：

```bash
cargo run -p kt-alarm-clock
```

Windows 构建使用 GUI subsystem，不弹出控制台窗口；诊断时可从终端运行测试或临时改用 Debug 构建。

## 测试

快速测试：

```bash
cargo test --workspace
cargo clippy --workspace --all-targets -- -D warnings
```

独立 Release 进程资源测试：

```bash
cargo test --release -p kt-alarm-clock --test resource_usage \
  release_resource_usage -- --ignored --nocapture --test-threads=1
```

资源结果同时打印并写入 `target/resource-usage.json`。测试口径和当前 macOS 基线见 [docs/资源测试.md](docs/资源测试.md)。

## 配置与自启动

配置统一使用 TOML 文件：

- Windows：`%APPDATA%\KuntaiSoft\KtAlarmClock\config.toml`
- macOS：`~/Library/Application Support/KuntaiSoft/KtAlarmClock/config.toml`

Windows 升级时，如果 TOML 尚不存在，会一次性读取旧版 `HKCU\SOFTWARE\KuntaiSoft\KtAlarmClock` 的计时参数；成功写入 TOML 后不再把注册表作为配置真值。

自启动只在用户设置改变或启动项与目标状态不一致时同步：

- Windows：当前用户 Run 项；
- macOS：`~/Library/LaunchAgents/com.kuntaisoft.KtAlarmClock.plist`。

## 代码结构

```text
app/                         Slint UI、桌面事件循环、窗口编排、资源集成测试
crates/clock-domain/         纯计时状态机与设置校验
crates/clock-app/            Controller、Effect、TOML 配置存储
crates/clock-platform/       Windows/macOS 单实例、休眠、自启动、窗口与资源适配
image/                       Slint 实际使用的托盘、休息与设置动作 SVG
docs/                        设计、验收和迁移调研
```

详细入口见 [docs/README.md](docs/README.md)，迁移决策与实测见 [docs/Clock去Qt与Rust重写可行性分析.md](docs/Clock去Qt与Rust重写可行性分析.md)。

## 当前验证边界

macOS arm64 已完成编译、严格 Clippy、自动测试、Release 资源采样、单实例和遮罩启动 smoke test。Windows 平台代码已通过 Rust 1.96.1 的 `x86_64-pc-windows-msvc` 全工作区交叉检查，并接入 Windows CI；发布前仍必须在 Windows CI/实机复核链接运行、真实休眠、自启动、双/三屏、混合 DPI 与资源指标。详细方案见 [docs/Windows与CI验证.md](docs/Windows与CI验证.md)。

## License

[Apache-2.0](LICENSE)
