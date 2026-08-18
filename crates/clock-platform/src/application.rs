use crate::PlatformResult;

/// 配置桌面应用进程自身的原生行为。
pub fn configure_application() -> PlatformResult<()> {
    imp::configure()
}

/// 在不恢复 Dock 图标的前提下激活应用并将现有窗口带到当前桌面。
pub fn activate_application() {
    imp::activate();
}

#[cfg(target_os = "macos")]
mod imp {
    use super::*;
    use objc2::MainThreadMarker;
    use objc2_app_kit::{NSApplication, NSApplicationActivationPolicy};

    pub(super) fn configure() -> PlatformResult<()> {
        let main_thread = MainThreadMarker::new().ok_or("必须在 macOS 主线程配置应用")?;
        let application = NSApplication::sharedApplication(main_thread);
        if !application.setActivationPolicy(NSApplicationActivationPolicy::Accessory) {
            return Err("macOS 拒绝隐藏 Dock 应用图标".into());
        }
        Ok(())
    }

    pub(super) fn activate() {
        let Some(main_thread) = MainThreadMarker::new() else {
            return;
        };
        NSApplication::sharedApplication(main_thread).activate();
    }
}

#[cfg(not(target_os = "macos"))]
mod imp {
    use super::*;

    pub(super) fn configure() -> PlatformResult<()> {
        Ok(())
    }

    pub(super) fn activate() {}
}
