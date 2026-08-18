use crate::PlatformResult;

/// 与 UI toolkit 无关的显示器物理矩形；仅在 winit 枚举失败时使用。
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct MonitorGeometry {
    pub x: i32,
    pub y: i32,
    pub width: u32,
    pub height: u32,
    pub scale_bits: u64,
}

pub fn native_monitor_geometries() -> PlatformResult<Vec<MonitorGeometry>> {
    imp::enumerate()
}

#[cfg(target_os = "macos")]
mod imp {
    use super::*;
    use objc2::MainThreadMarker;
    use objc2_app_kit::NSScreen;

    pub(super) fn enumerate() -> PlatformResult<Vec<MonitorGeometry>> {
        let mtm = MainThreadMarker::new().ok_or("NSScreen 必须在 macOS 主线程枚举")?;
        let screens = NSScreen::screens(mtm);
        let Some(main) = NSScreen::mainScreen(mtm) else {
            return Ok(Vec::new());
        };
        let main_frame = main.frame();
        let main_left = main_frame.origin.x;
        let main_top = main_frame.origin.y + main_frame.size.height;

        Ok(screens
            .iter()
            .filter_map(|screen| {
                let frame = screen.frame();
                let scale = screen.backingScaleFactor();
                let width = positive_u32(frame.size.width * scale)?;
                let height = positive_u32(frame.size.height * scale)?;
                Some(MonitorGeometry {
                    x: rounded_i32((frame.origin.x - main_left) * scale),
                    y: rounded_i32((main_top - (frame.origin.y + frame.size.height)) * scale),
                    width,
                    height,
                    scale_bits: scale.to_bits(),
                })
            })
            .collect())
    }

    fn rounded_i32(value: f64) -> i32 {
        value.round().clamp(i32::MIN as f64, i32::MAX as f64) as i32
    }

    fn positive_u32(value: f64) -> Option<u32> {
        let value = value.round();
        (value > 0.0).then(|| value.min(u32::MAX as f64) as u32)
    }
}

#[cfg(target_os = "windows")]
#[allow(unsafe_code)]
mod imp {
    use super::*;
    use std::mem::size_of;
    use windows::Win32::Foundation::{LPARAM, RECT};
    use windows::Win32::Graphics::Gdi::{
        EnumDisplayMonitors, GetMonitorInfoW, HDC, HMONITOR, MONITORINFO,
    };
    use windows::core::BOOL;

    pub(super) fn enumerate() -> PlatformResult<Vec<MonitorGeometry>> {
        let mut monitors = Vec::new();
        // SAFETY: `monitors` remains alive and exclusively borrowed until the synchronous
        // EnumDisplayMonitors call returns. The callback validates GetMonitorInfoW.
        let success = unsafe {
            EnumDisplayMonitors(
                None,
                None,
                Some(collect_monitor),
                LPARAM((&mut monitors as *mut Vec<MonitorGeometry>) as isize),
            )
        };
        success.ok()?;
        Ok(monitors)
    }

    unsafe extern "system" fn collect_monitor(
        monitor: HMONITOR,
        _device_context: HDC,
        _clip: *mut RECT,
        data: LPARAM,
    ) -> BOOL {
        let mut info = MONITORINFO {
            cbSize: size_of::<MONITORINFO>() as u32,
            ..Default::default()
        };
        // SAFETY: `monitor` is supplied by EnumDisplayMonitors and `info` has the required size.
        if !unsafe { GetMonitorInfoW(monitor, &mut info) }.as_bool() {
            return BOOL(1);
        }
        let width = info.rcMonitor.right.saturating_sub(info.rcMonitor.left);
        let height = info.rcMonitor.bottom.saturating_sub(info.rcMonitor.top);
        if width <= 0 || height <= 0 {
            return BOOL(1);
        }

        // SAFETY: `data` was created from an exclusive Vec pointer for this synchronous call.
        let output = unsafe { &mut *(data.0 as *mut Vec<MonitorGeometry>) };
        output.push(MonitorGeometry {
            x: info.rcMonitor.left,
            y: info.rcMonitor.top,
            width: width as u32,
            height: height as u32,
            scale_bits: 1_f64.to_bits(),
        });
        BOOL(1)
    }
}

#[cfg(not(any(target_os = "macos", target_os = "windows")))]
mod imp {
    use super::*;

    pub(super) fn enumerate() -> PlatformResult<Vec<MonitorGeometry>> {
        Ok(Vec::new())
    }
}
