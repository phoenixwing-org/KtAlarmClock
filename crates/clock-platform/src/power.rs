use crate::PlatformResult;
use std::time::{Instant, SystemTime};

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum PowerEvent {
    Suspended {
        monotonic: Instant,
        wall: SystemTime,
    },
    Resumed {
        monotonic: Instant,
        wall: SystemTime,
    },
}

impl PowerEvent {
    #[cfg(any(target_os = "macos", target_os = "windows"))]
    fn suspended_now() -> Self {
        Self::Suspended {
            monotonic: Instant::now(),
            wall: SystemTime::now(),
        }
    }

    #[cfg(any(target_os = "macos", target_os = "windows"))]
    fn resumed_now() -> Self {
        Self::Resumed {
            monotonic: Instant::now(),
            wall: SystemTime::now(),
        }
    }
}

pub struct PowerMonitor(imp::PowerMonitor);

impl PowerMonitor {
    pub fn new() -> PlatformResult<Self> {
        Ok(Self(imp::PowerMonitor::new()?))
    }

    pub fn drain(&self) -> Vec<PowerEvent> {
        self.0.drain()
    }
}

#[cfg(target_os = "macos")]
#[allow(unsafe_code)]
mod imp {
    use super::*;
    use block2::RcBlock;
    use objc2::rc::Retained;
    use objc2::runtime::{AnyObject, ProtocolObject};
    use objc2_app_kit::{
        NSWorkspace, NSWorkspaceDidWakeNotification, NSWorkspaceWillSleepNotification,
    };
    use objc2_foundation::{NSNotification, NSNotificationCenter, NSObjectProtocol};
    use std::ptr::NonNull;
    use std::sync::mpsc::{self, Receiver, Sender};

    pub(super) struct PowerMonitor {
        receiver: Receiver<PowerEvent>,
        center: Retained<NSNotificationCenter>,
        observers: Vec<Retained<ProtocolObject<dyn NSObjectProtocol>>>,
    }

    impl PowerMonitor {
        pub(super) fn new() -> PlatformResult<Self> {
            let (sender, receiver) = mpsc::channel();
            let center = NSWorkspace::sharedWorkspace().notificationCenter();
            // SAFETY: These are immutable AppKit notification-name constants
            // available for the lifetime of the process.
            let (sleep_name, wake_name) = unsafe {
                (
                    NSWorkspaceWillSleepNotification,
                    NSWorkspaceDidWakeNotification,
                )
            };
            let sleep = add_observer(
                &center,
                sleep_name,
                sender.clone(),
                PowerEvent::suspended_now,
            );
            let wake = add_observer(&center, wake_name, sender, PowerEvent::resumed_now);
            Ok(Self {
                receiver,
                center,
                observers: vec![sleep, wake],
            })
        }

        pub(super) fn drain(&self) -> Vec<PowerEvent> {
            self.receiver.try_iter().collect()
        }
    }

    impl Drop for PowerMonitor {
        fn drop(&mut self) {
            for observer in &self.observers {
                // SAFETY: Tokens were returned by this exact notification center and
                // remain retained in `self.observers` until after removal.
                let observer: &AnyObject = AsRef::<AnyObject>::as_ref(&**observer);
                unsafe { self.center.removeObserver(observer) };
            }
        }
    }

    fn add_observer(
        center: &NSNotificationCenter,
        name: &objc2_foundation::NSNotificationName,
        sender: Sender<PowerEvent>,
        event: fn() -> PowerEvent,
    ) -> Retained<ProtocolObject<dyn NSObjectProtocol>> {
        let block = RcBlock::new(move |_notification: NonNull<NSNotification>| {
            let _ = sender.send(event());
        });
        // SAFETY: The name is an AppKit notification constant, `obj`/`queue` are
        // intentionally unrestricted, and RcBlock owns a Send-capable Sender.
        unsafe { center.addObserverForName_object_queue_usingBlock(Some(name), None, None, &block) }
    }
}

#[cfg(target_os = "windows")]
#[allow(unsafe_code)]
mod imp {
    use super::*;
    use std::sync::mpsc::{self, Receiver};
    use std::sync::{Mutex, OnceLock};
    use std::thread::{self, JoinHandle};
    use windows::Win32::Foundation::{HINSTANCE, HWND, LPARAM, LRESULT, WPARAM};
    use windows::Win32::System::LibraryLoader::GetModuleHandleW;
    use windows::Win32::System::Threading::GetCurrentThreadId;
    use windows::Win32::UI::WindowsAndMessaging::{
        CreateWindowExW, DefWindowProcW, DestroyWindow, DispatchMessageW, GetMessageW,
        HWND_MESSAGE, MSG, PBT_APMRESUMEAUTOMATIC, PBT_APMRESUMECRITICAL, PBT_APMRESUMESUSPEND,
        PBT_APMSUSPEND, PostThreadMessageW, RegisterClassW, TranslateMessage, WINDOW_EX_STYLE,
        WINDOW_STYLE, WM_POWERBROADCAST, WM_QUIT, WNDCLASSW,
    };
    use windows::core::w;

    static POWER_SENDER: OnceLock<Mutex<Option<mpsc::Sender<PowerEvent>>>> = OnceLock::new();

    pub(super) struct PowerMonitor {
        receiver: Receiver<PowerEvent>,
        thread_id: u32,
        thread: Option<JoinHandle<()>>,
    }

    impl PowerMonitor {
        pub(super) fn new() -> PlatformResult<Self> {
            let (event_sender, receiver) = mpsc::channel();
            let (setup_sender, setup_receiver) = mpsc::sync_channel(1);
            let thread = thread::Builder::new()
                .name("clock-power-events".into())
                .spawn(move || run_message_window(event_sender, setup_sender))?;
            let thread_id = setup_receiver.recv()?.map_err(std::io::Error::other)?;
            Ok(Self {
                receiver,
                thread_id,
                thread: Some(thread),
            })
        }

        pub(super) fn drain(&self) -> Vec<PowerEvent> {
            self.receiver.try_iter().collect()
        }
    }

    impl Drop for PowerMonitor {
        fn drop(&mut self) {
            // SAFETY: `thread_id` is reported only after the message queue and
            // hidden window exist. WM_QUIT asks that thread's GetMessage loop to end.
            let _ = unsafe { PostThreadMessageW(self.thread_id, WM_QUIT, WPARAM(0), LPARAM(0)) };
            if let Some(thread) = self.thread.take() {
                let _ = thread.join();
            }
        }
    }

    fn run_message_window(
        sender: mpsc::Sender<PowerEvent>,
        setup_sender: mpsc::SyncSender<Result<u32, String>>,
    ) {
        // SAFETY: All Win32 objects are created and consumed on this dedicated
        // thread. The callback reaches the Sender through a process-local mutex.
        let setup = unsafe {
            let module = match GetModuleHandleW(None) {
                Ok(module) => module,
                Err(error) => {
                    let _ = setup_sender.send(Err(error.to_string()));
                    return;
                }
            };
            let class_name = w!("KtAlarmClockPowerEvents");
            let class = WNDCLASSW {
                hInstance: HINSTANCE(module.0),
                lpszClassName: class_name,
                lpfnWndProc: Some(window_proc),
                ..Default::default()
            };
            // An already-registered process-local class is also usable; any
            // real registration failure is reported by CreateWindowExW below.
            let _ = RegisterClassW(&class);

            *POWER_SENDER
                .get_or_init(|| Mutex::new(None))
                .lock()
                .expect("电源事件 Sender mutex 中毒") = Some(sender);
            let window = match CreateWindowExW(
                WINDOW_EX_STYLE::default(),
                class_name,
                w!(""),
                WINDOW_STYLE::default(),
                0,
                0,
                0,
                0,
                Some(HWND_MESSAGE),
                None,
                Some(HINSTANCE(module.0)),
                None,
            ) {
                Ok(window) => window,
                Err(error) => {
                    *POWER_SENDER
                        .get()
                        .expect("电源事件 Sender 尚未初始化")
                        .lock()
                        .expect("电源事件 Sender mutex 中毒") = None;
                    let _ = setup_sender.send(Err(error.to_string()));
                    return;
                }
            };
            (window, GetCurrentThreadId())
        };

        let (window, thread_id) = setup;
        if setup_sender.send(Ok(thread_id)).is_err() {
            let _ = unsafe { DestroyWindow(window) };
            return;
        }

        unsafe {
            let mut message = MSG::default();
            while GetMessageW(&mut message, None, 0, 0).as_bool() {
                let _ = TranslateMessage(&message);
                DispatchMessageW(&message);
            }
            let _ = DestroyWindow(window);
            *POWER_SENDER
                .get()
                .expect("电源事件 Sender 尚未初始化")
                .lock()
                .expect("电源事件 Sender mutex 中毒") = None;
        }
    }

    unsafe extern "system" fn window_proc(
        window: HWND,
        message: u32,
        wparam: WPARAM,
        lparam: LPARAM,
    ) -> LRESULT {
        if message == WM_POWERBROADCAST {
            let event = match wparam.0 as u32 {
                PBT_APMSUSPEND => Some(PowerEvent::suspended_now()),
                PBT_APMRESUMEAUTOMATIC | PBT_APMRESUMESUSPEND | PBT_APMRESUMECRITICAL => {
                    Some(PowerEvent::resumed_now())
                }
                _ => None,
            };
            if let Some(event) = event
                && let Some(sender) = POWER_SENDER
                    .get()
                    .and_then(|sender| sender.lock().ok())
                    .and_then(|sender| sender.clone())
            {
                let _ = sender.send(event);
            }
        }

        unsafe { DefWindowProcW(window, message, wparam, lparam) }
    }
}

#[cfg(not(any(target_os = "macos", target_os = "windows")))]
mod imp {
    use super::*;

    pub(super) struct PowerMonitor;

    impl PowerMonitor {
        pub(super) fn new() -> PlatformResult<Self> {
            Ok(Self)
        }

        pub(super) fn drain(&self) -> Vec<PowerEvent> {
            Vec::new()
        }
    }
}
