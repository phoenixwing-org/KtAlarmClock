# Windows 与 CI 验证方案

> 调研日期：2026-08-11
> 范围：说明当前 Windows 代码验证到什么程度、macOS 能完成哪些 Windows 检查，以及 GitHub Actions 与 Gitee Go 如何选择。

## 1. 结论

当前推荐方案是：

1. Gitee 继续作为主要代码仓库；
2. GitHub 建立公开镜像，只用于 Windows/macOS CI；
3. GitHub 使用标准 `windows-2022` runner 执行真实 MSVC 链接、测试和 Release 构建；
4. macOS 本机继续执行 Windows 目标交叉检查，尽早发现条件编译和 Win32 API 类型问题；
5. 发布前仍使用 Windows 实机完成休眠、多屏、混合 DPI、自启动与资源指标验收。

Gitee Go 有流水线服务，但当前公开官方文档没有列出可像 GitHub `windows-2022` 一样直接选择的托管 Windows 构建镜像。若通过 Gitee 完成真正的 MSVC 构建，通常还要准备并接入自有 Windows 主机，因此本项目使用 GitHub Actions 更直接。

## 2. 当前仓库状态

当前远端只有 Gitee：

```text
origin  https://gitee.com/PhoenixWing321/KtAlarmClock.git
```

仓库已经包含 GitHub Actions 配置：

```text
.github/workflows/rust.yml
```

该文件配置了 `macos-14` 与 `windows-2022` 两个平台，但只有代码被推送到 GitHub 仓库后才会运行；Gitee 不读取 `.github/workflows/`。

当前 Windows 验证已经完成：

```bash
cargo check --workspace --target x86_64-pc-windows-msvc
```

实际执行时使用了与项目匹配的 Rust 1.96.1 Windows 标准库。交叉检查发现并修正过 `windows` 0.62 的 `BOOL` 导入、Win32 错误转换和条件导入问题，因此不是单纯的源码目测。

## 3. `x86_64-pc-windows-msvc` 的含义

```text
x86_64   64 位 Intel/AMD CPU
pc       普通 PC 平台标识
windows  Windows 操作系统
msvc     微软 ABI、运行库和链接工具体系
```

这是绝大多数 64 位 Windows 桌面程序的首选 Rust 目标。它与 Visual Studio Build Tools、Windows SDK 和 Win32 API 的兼容性最好。

macOS 上的 `cargo check` 会编译 Windows 条件代码并检查类型，但不会完成以下工作：

- 不调用微软 linker 生成最终 `.exe`；
- 不启动 Windows 程序；
- 不验证系统托盘、注册表和窗口实际行为；
- 不产生可信的 Windows 内存与 CPU 数据。

## 4. macOS 能做哪些 Windows 测试

| 项目 | macOS 本机 | 说明 |
| --- | --- | --- |
| Windows 条件编译与 Win32 API 类型检查 | 可以 | 当前已经通过 |
| Windows Release 最终链接 | 可以补充 | 可评估 `cargo-xwin`，但当前尚未接入 |
| 执行 Windows 单元测试 | 不能原生执行 | 交给 Windows CI 或虚拟机 |
| UI、托盘、单实例 smoke test | 虚拟机可做 | Apple Silicon 建议 Windows 11 ARM 虚拟机 |
| 注册表自启动 | 虚拟机可做 | 需要实际登录、退出和重启验证 |
| 真实休眠/唤醒通知 | 虚拟机不可靠 | 虚拟机电源事件不等同物理 Windows 机器 |
| 双/三屏同步遮罩与混合 DPI | 虚拟机仅能初测 | 最终必须使用真实显示器布局 |
| Windows 内存、CPU 与发布体积基线 | 不可替代 | 应在固定 Windows 实机采样 |

Wine 不作为最终验收环境。它不能可靠复现 Windows 登录启动、真实电源通知、多屏 DPI 和 Win32 topmost 竞争行为。

## 5. GitHub Actions

### 5.1 是否需要申请 Windows 资源

不需要单独申请 Azure 虚拟机或 Windows 许可证。代码触发 workflow 后，GitHub 会按需分配标准 `windows-2022` runner。

- 公开仓库使用标准 GitHub-hosted runner 免费且不限分钟；
- 私有仓库使用账户套餐包含的 Actions 分钟，GitHub Free 当前列出每月 2,000 分钟；
- 组织策略可能默认禁用 Actions，需要仓库或组织管理员开启。

额度规则可能调整，实际使用前以 GitHub 官方页面为准。

### 5.2 当前 workflow 的验证能力

当前 `.github/workflows/rust.yml` 已执行：

```bash
cargo check --workspace
cargo test --workspace
cargo clippy --workspace --all-targets -- -D warnings
```

在 Windows runner 上，`cargo test` 会真正调用 MSVC/Windows SDK 链接测试程序并运行测试。但为了明确验证最终优化后的 GUI 可执行文件，还应增加：

```yaml
- name: Build release application
  run: cargo build --workspace --release
```

可进一步上传 `target/release/KtAlarmClock.exe` 作为 CI artifact，供人工 smoke test；这属于后续发布流水线工作，不影响当前源码验证结论。

### 5.3 建议的双远端方式

保留现有 Gitee `origin`，新增 GitHub remote，不改变日常 Gitee 工作方式：

```bash
git remote add github git@github.com:<账号>/KtAlarmClock.git
git push -u github rust
```

上述命令只是建议；创建 GitHub 仓库、添加远端和首次推送属于外部发布操作，执行前需要用户明确授权。

## 6. Gitee Go

Gitee Go 是 Gitee 的 CI/CD 流水线服务，支持 push、PR、定时触发、构建记录和制品管理。

当前官方资料说明：

- 社区版和企业版可开通 Gitee Go；
- 开通时可能要求账号绑定手机号；
- 单个仓库提供 200 分钟一次性免费构建时长；
- 个人、组织或企业每月提供 1,000 分钟免费构建时长；
- 流水线描述放在 `.workflow/`，不能直接复用 GitHub 的 `.github/workflows/`；
- Windows 和 macOS 主机可以通过命令逐台加入主机组。

最后一项表示 Gitee 可以接入用户自己的 Windows 机器，但它不等于平台已经提供托管 Windows/MSVC runner。对于本项目，如果还要自行维护 Windows 主机、Agent、在线状态和工具链，复杂度会高于直接使用 GitHub `windows-2022`。

## 7. 推荐取舍

| 方案 | Windows 真实链接 | 维护成本 | 本项目结论 |
| --- | --- | --- | --- |
| macOS `cargo check` | 否 | 低 | 保留为日常快速门禁 |
| GitHub Actions `windows-2022` | 是 | 低 | 推荐作为主要 Windows CI |
| Gitee Go 云端构建 | 官方未明确提供托管 Windows 镜像 | 中 | 可做 Linux 类任务，不解决当前关键问题 |
| Gitee Go + 自有 Windows 主机 | 是 | 高 | 有长期在线 Windows 机器时可选 |
| Windows 11 ARM 虚拟机 | 是 | 中 | 适合本地 UI smoke test，不替代实机资源与多屏验收 |
| Windows x64 实机 | 是 | 中 | 发布前最终验收必需 |

## 8. Windows 发布前验收清单

CI 自动门禁：

- Windows MSVC Debug/测试链接成功；
- `cargo test --workspace` 全部通过；
- 严格 Clippy 通过；
- `cargo build --workspace --release` 生成 `KtAlarmClock.exe`；
- 发布依赖中不存在 Qt、WebView2、CEF/Chromium。

Windows 实机验收：

- 系统托盘、主胶囊、设置窗、单实例激活；
- Run 注册表自启动开启、关闭与重复启动不重写；
- 工作中休眠后冻结，唤醒后从冻结点继续；
- 休息中休眠计入休息，唤醒后立即允许按当前时间解锁；
- 双屏、三屏首次进入休息时同批覆盖；
- 显示器热插拔时补齐或销毁遮罩；
- 100%、150%、200% 与混合 DPI；
- 遮罩置顶、切换应用、任务栏与失焦场景；
- stripped Release 体积、空闲/休息 private memory 与 CPU。

## 8.1 Windows 手工点检清单（窗口定位与回归）

**日期：2026-08-18（本地无法执行，请补充到 Windows 实机）**

1. 首次启动主计时窗口：
   - 位于主显示器上边缘靠右（约 24px 间距）。
   - 不要出现半截屏外区域。
2. 拖动主计时窗口到任意位置后退出程序，再重启：
   - 退出前最后位置应自动恢复。
3. 手动拖动窗口到主显示器左/上边界外 30px：
   - 再次显示（含托盘“显示”）后应自动回拉到屏幕可见区域。
4. 多显示器环境：
   - 回到上一次位置时，始终基于保存坐标，不跨屏跳位。
5. 休眠唤醒后：
   - 计时主窗体仍保持记忆位置，无明显抖动。

备注：上述项属于 `main` window 的人工验收点，记录到本分支提交后续 PR 的验收注释。

## 9. 官方资料

- [GitHub-hosted runners](https://docs.github.com/en/actions/reference/runners/github-hosted-runners)
- [GitHub 各套餐包含的 Actions 用量](https://docs.github.com/en/billing/reference/product-usage-included)
- [Gitee Go 三步开启流水线](https://gitee.com/help/articles/4357)
- [Gitee Go 产品定价和计费模型](https://gitee.com/help/articles/4365)
- [Gitee Go 流水线上手与 `.workflow` 文件](https://gitee.com/help/articles/4381)
- [Gitee Go 多云主机组管理](https://gitee.com/help/articles/4363)
