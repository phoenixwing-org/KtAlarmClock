use crate::PlatformResult;
use raw_window_handle::HasWindowHandle;

pub fn configure_overlay_window(window: &impl HasWindowHandle) -> PlatformResult<()> {
    imp::configure_overlay(window)
}

/// 配置常驻计时胶囊。它高于普通应用窗口，但低于休息遮罩。
pub fn configure_main_window(window: &impl HasWindowHandle) -> PlatformResult<()> {
    imp::configure_main(window)
}

/// 将计时胶囊限制在当前显示器的可用工作区内。
pub fn constrain_main_window(window: &impl HasWindowHandle) -> PlatformResult<()> {
    imp::constrain_main(window)
}

fn clamp_coordinate(position: f64, size: f64, available_start: f64, available_size: f64) -> f64 {
    if size >= available_size {
        return available_start;
    }
    position.clamp(available_start, available_start + available_size - size)
}

#[cfg(target_os = "macos")]
#[allow(unsafe_code)]
mod imp {
    use super::*;
    use objc2::Message;
    use objc2_app_kit::{
        NSColor, NSFloatingWindowLevel, NSScreen, NSScreenSaverWindowLevel, NSView,
        NSWindowCollectionBehavior,
    };
    use objc2_foundation::NSPoint;
    use raw_window_handle::RawWindowHandle;

    // The 34 logical-pixel capsule is a pill: radius equals half its height.
    const MAIN_CORNER_RADIUS: f64 = 17.0;

    pub(super) fn configure_main(window: &impl HasWindowHandle) -> PlatformResult<()> {
        let (native_window, view) = native_objects(window)?;
        let behavior = native_window.collectionBehavior()
            | NSWindowCollectionBehavior::CanJoinAllSpaces
            | NSWindowCollectionBehavior::FullScreenAuxiliary
            | NSWindowCollectionBehavior::Stationary;
        native_window.setCollectionBehavior(behavior);
        native_window.setLevel(NSFloatingWindowLevel);
        native_window.setOpaque(false);
        native_window.setBackgroundColor(Some(&NSColor::clearColor()));
        native_window.setHasShadow(false);

        // softbuffer's Core Graphics backend currently publishes an opaque CGImage
        // (`NoneSkipFirst`), so the transparent Slint pixels alone become black. Clip
        // the layer-backed content view to the same radius as the Slint Rectangle.
        view.setWantsLayer(true);
        let layer = view.layer().ok_or("NSView 无法创建 CALayer")?;
        layer.setCornerRadius(MAIN_CORNER_RADIUS);
        layer.setMasksToBounds(true);
        // Accessory applications have no Dock activation path. Explicitly order the
        // already-shown capsule to the front so it remains visible on the active Space.
        native_window.orderFrontRegardless();
        constrain_native_window(&native_window)
    }

    pub(super) fn constrain_main(window: &impl HasWindowHandle) -> PlatformResult<()> {
        let (native_window, _view) = native_objects(window)?;
        constrain_native_window(&native_window)
    }

    pub(super) fn configure_overlay(window: &impl HasWindowHandle) -> PlatformResult<()> {
        let (native_window, _view) = native_objects(window)?;
        let behavior = native_window.collectionBehavior()
            | NSWindowCollectionBehavior::CanJoinAllSpaces
            | NSWindowCollectionBehavior::FullScreenAuxiliary
            | NSWindowCollectionBehavior::Stationary;
        native_window.setCollectionBehavior(behavior);
        if let Some(screen) = native_window.screen() {
            let frame = screen.frame();
            // Slint may still deliver its ordinary initial-size request after
            // simple fullscreen. Lock both native constraints to the screen so
            // AppKit cannot shrink a lock overlay back to the 90% normal-window
            // size. The overlay window is destroyed on dismissal.
            native_window.setContentMinSize(frame.size);
            native_window.setContentMaxSize(frame.size);
            native_window.setMinSize(frame.size);
            native_window.setMaxSize(frame.size);
            native_window.setFrame_display(frame, true);
        }
        // The software renderer can publish a smaller damage surface briefly on
        // scaled macOS displays. Give the full native window the exact overlay
        // color so no desktop can show around that surface during the resize.
        let background = NSColor::colorWithSRGBRed_green_blue_alpha(
            31.0 / 255.0,
            41.0 / 255.0,
            55.0 / 255.0,
            1.0,
        );
        native_window.setBackgroundColor(Some(&background));
        native_window.setOpaque(true);
        native_window.setLevel(NSScreenSaverWindowLevel);
        native_window.orderFrontRegardless();
        Ok(())
    }

    fn constrain_native_window(window: &objc2_app_kit::NSWindow) -> PlatformResult<()> {
        let main_thread = objc2::MainThreadMarker::new().ok_or("必须在 macOS 主线程约束窗口")?;
        let screen = window
            .screen()
            .or_else(|| NSScreen::mainScreen(main_thread))
            .ok_or("macOS 未提供可用显示器")?;
        let available = screen.visibleFrame();
        let frame = window.frame();
        let x = clamp_coordinate(
            frame.origin.x,
            frame.size.width,
            available.origin.x,
            available.size.width,
        );
        let y = clamp_coordinate(
            frame.origin.y,
            frame.size.height,
            available.origin.y,
            available.size.height,
        );
        if x != frame.origin.x || y != frame.origin.y {
            window.setFrameOrigin(NSPoint::new(x, y));
        }
        Ok(())
    }

    fn native_objects(
        window: &impl HasWindowHandle,
    ) -> PlatformResult<(
        objc2::rc::Retained<objc2_app_kit::NSWindow>,
        objc2::rc::Retained<NSView>,
    )> {
        let handle = window
            .window_handle()
            .map_err(|error| format!("读取 AppKit 窗口句柄失败: {error:?}"))?;
        let RawWindowHandle::AppKit(handle) = handle.as_raw() else {
            return Err("Slint/winit 未提供 AppKit 窗口句柄".into());
        };

        // SAFETY: raw-window-handle guarantees `ns_view` points to the live NSView
        // owned by this winit window for the duration of the borrowed handle.
        let view = unsafe { handle.ns_view.cast::<NSView>().as_ref() }.retain();
        let native_window = view.window().ok_or("NSView 尚未挂接 NSWindow")?;
        Ok((native_window, view))
    }
}

#[cfg(target_os = "windows")]
#[allow(unsafe_code)]
mod imp {
    use super::*;
    use raw_window_handle::RawWindowHandle;
    use windows::Win32::Foundation::{HWND, RECT};
    use windows::Win32::Graphics::Gdi::{
        CreateRoundRectRgn, GetMonitorInfoW, HGDIOBJ, MONITOR_DEFAULTTONEAREST, MONITORINFO,
        MonitorFromWindow, SetWindowRgn,
    };
    use windows::Win32::UI::HiDpi::GetDpiForWindow;
    use windows::Win32::UI::WindowsAndMessaging::{
        GetWindowRect, HWND_TOPMOST, SWP_NOACTIVATE, SWP_NOMOVE, SWP_NOSIZE, SWP_NOZORDER,
        SWP_SHOWWINDOW, SetWindowPos,
    };

    // Keep the native window region aligned with the 34 logical-pixel Slint pill.
    const MAIN_CORNER_RADIUS: i32 = 17;

    pub(super) fn configure_main(window: &impl HasWindowHandle) -> PlatformResult<()> {
        let window = native_window(window)?;
        set_topmost(window)?;
        clip_to_rounded_rect(window)?;
        constrain_native_window(window)
    }

    pub(super) fn constrain_main(window: &impl HasWindowHandle) -> PlatformResult<()> {
        constrain_native_window(native_window(window)?)
    }

    pub(super) fn configure_overlay(window: &impl HasWindowHandle) -> PlatformResult<()> {
        set_topmost(native_window(window)?)
    }

    fn native_window(window: &impl HasWindowHandle) -> PlatformResult<HWND> {
        let handle = window
            .window_handle()
            .map_err(|error| format!("读取 Win32 窗口句柄失败: {error:?}"))?;
        let RawWindowHandle::Win32(handle) = handle.as_raw() else {
            return Err("Slint/winit 未提供 Win32 窗口句柄".into());
        };
        Ok(HWND(handle.hwnd.get() as *mut core::ffi::c_void))
    }

    fn set_topmost(window: HWND) -> PlatformResult<()> {
        // SAFETY: HWND comes from the live winit window and SetWindowPos does not
        // retain it. Position/size are left unchanged; only topmost state is applied.
        unsafe {
            SetWindowPos(
                window,
                Some(HWND_TOPMOST),
                0,
                0,
                0,
                0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW,
            )
        }?;
        Ok(())
    }

    fn clip_to_rounded_rect(window: HWND) -> PlatformResult<()> {
        let mut bounds = RECT::default();
        // SAFETY: HWND is live and `bounds` is valid writable storage.
        unsafe { GetWindowRect(window, &mut bounds) }?;
        let width = bounds.right.saturating_sub(bounds.left);
        let height = bounds.bottom.saturating_sub(bounds.top);
        // SAFETY: Query only; `window` remains live for this call.
        let dpi = unsafe { GetDpiForWindow(window) }.max(96) as i32;
        let radius = (MAIN_CORNER_RADIUS * dpi + 48) / 96;
        // Right/bottom are exclusive; the extra pixel avoids clipping the final edge.
        // SAFETY: The dimensions are bounded by the current native window.
        let region = unsafe {
            CreateRoundRectRgn(
                0,
                0,
                width.saturating_add(1),
                height.saturating_add(1),
                radius.saturating_mul(2),
                radius.saturating_mul(2),
            )
        };
        if region.is_invalid() {
            return Err("创建 Windows 胶囊圆角区域失败".into());
        }

        // SAFETY: On success Windows owns `region`; on failure ownership stays here.
        if unsafe { SetWindowRgn(window, Some(region), true) } == 0 {
            // SAFETY: SetWindowRgn failed, so the region is still ours to release.
            let _ = unsafe { windows::Win32::Graphics::Gdi::DeleteObject(HGDIOBJ(region.0)) };
            return Err("设置 Windows 胶囊圆角区域失败".into());
        }
        Ok(())
    }

    fn constrain_native_window(window: HWND) -> PlatformResult<()> {
        // SAFETY: HWND comes from the live winit window. The returned HMONITOR is used
        // synchronously and MONITOR_DEFAULTTONEAREST also handles partially off-screen windows.
        let monitor = unsafe { MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST) };
        let mut info = MONITORINFO {
            cbSize: std::mem::size_of::<MONITORINFO>() as u32,
            ..Default::default()
        };
        // SAFETY: `monitor` is valid for this query and `info` has the documented size.
        unsafe { GetMonitorInfoW(monitor, &mut info) }.ok()?;

        let mut frame = RECT::default();
        // SAFETY: HWND is live and `frame` is writable for the duration of the call.
        unsafe { GetWindowRect(window, &mut frame) }?;
        let width = frame.right.saturating_sub(frame.left);
        let height = frame.bottom.saturating_sub(frame.top);
        let work_width = info.rcWork.right.saturating_sub(info.rcWork.left);
        let work_height = info.rcWork.bottom.saturating_sub(info.rcWork.top);
        let x = clamp_coordinate(
            f64::from(frame.left),
            f64::from(width),
            f64::from(info.rcWork.left),
            f64::from(work_width),
        )
        .round() as i32;
        let y = clamp_coordinate(
            f64::from(frame.top),
            f64::from(height),
            f64::from(info.rcWork.top),
            f64::from(work_height),
        )
        .round() as i32;
        if x != frame.left || y != frame.top {
            // SAFETY: Only the position changes; size and z-order remain untouched.
            unsafe {
                SetWindowPos(
                    window,
                    None,
                    x,
                    y,
                    0,
                    0,
                    SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE,
                )
            }?;
        }
        Ok(())
    }
}

#[cfg(not(any(target_os = "macos", target_os = "windows")))]
mod imp {
    use super::*;

    pub(super) fn configure_main(_window: &impl HasWindowHandle) -> PlatformResult<()> {
        Ok(())
    }

    pub(super) fn constrain_main(_window: &impl HasWindowHandle) -> PlatformResult<()> {
        Ok(())
    }

    pub(super) fn configure_overlay(_window: &impl HasWindowHandle) -> PlatformResult<()> {
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::clamp_coordinate;

    #[test]
    fn clamps_all_edges_to_available_area() {
        assert_eq!(clamp_coordinate(-20.0, 80.0, 0.0, 1_000.0), 0.0);
        assert_eq!(clamp_coordinate(980.0, 80.0, 0.0, 1_000.0), 920.0);
        assert_eq!(clamp_coordinate(200.0, 80.0, 0.0, 1_000.0), 200.0);
        assert_eq!(clamp_coordinate(20.0, 1_200.0, 0.0, 1_000.0), 0.0);
    }
}
