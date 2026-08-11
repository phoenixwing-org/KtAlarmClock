use crate::PlatformResult;
use raw_window_handle::HasWindowHandle;

pub fn configure_overlay_window(window: &impl HasWindowHandle) -> PlatformResult<()> {
    imp::configure(window)
}

#[cfg(target_os = "macos")]
#[allow(unsafe_code)]
mod imp {
    use super::*;
    use objc2_app_kit::{NSScreenSaverWindowLevel, NSView, NSWindowCollectionBehavior};
    use raw_window_handle::RawWindowHandle;

    pub(super) fn configure(window: &impl HasWindowHandle) -> PlatformResult<()> {
        let handle = window
            .window_handle()
            .map_err(|error| format!("读取 AppKit 窗口句柄失败: {error:?}"))?;
        let RawWindowHandle::AppKit(handle) = handle.as_raw() else {
            return Err("Slint/winit 未提供 AppKit 窗口句柄".into());
        };

        // SAFETY: raw-window-handle guarantees `ns_view` points to the live NSView
        // owned by this winit window for the duration of the borrowed handle.
        let view = unsafe { handle.ns_view.cast::<NSView>().as_ref() };
        let native_window = view.window().ok_or("NSView 尚未挂接 NSWindow")?;
        let behavior = native_window.collectionBehavior()
            | NSWindowCollectionBehavior::CanJoinAllSpaces
            | NSWindowCollectionBehavior::FullScreenAuxiliary
            | NSWindowCollectionBehavior::Stationary;
        native_window.setCollectionBehavior(behavior);
        native_window.setLevel(NSScreenSaverWindowLevel);
        Ok(())
    }
}

#[cfg(target_os = "windows")]
#[allow(unsafe_code)]
mod imp {
    use super::*;
    use raw_window_handle::RawWindowHandle;
    use windows::Win32::Foundation::HWND;
    use windows::Win32::UI::WindowsAndMessaging::{
        HWND_TOPMOST, SWP_NOACTIVATE, SWP_NOMOVE, SWP_NOSIZE, SWP_SHOWWINDOW, SetWindowPos,
    };

    pub(super) fn configure(window: &impl HasWindowHandle) -> PlatformResult<()> {
        let handle = window
            .window_handle()
            .map_err(|error| format!("读取 Win32 窗口句柄失败: {error:?}"))?;
        let RawWindowHandle::Win32(handle) = handle.as_raw() else {
            return Err("Slint/winit 未提供 Win32 窗口句柄".into());
        };
        let window = HWND(handle.hwnd.get() as *mut core::ffi::c_void);
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
}

#[cfg(not(any(target_os = "macos", target_os = "windows")))]
mod imp {
    use super::*;

    pub(super) fn configure(_window: &impl HasWindowHandle) -> PlatformResult<()> {
        Ok(())
    }
}
