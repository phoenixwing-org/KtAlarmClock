#![cfg_attr(target_os = "windows", windows_subsystem = "windows")]

use clock_app::{Controller, Effect, Event, FileConfigStore, UserAction};
use clock_domain::{Phase, Settings, Snapshot, TimePoint};
use clock_platform::{
    InstanceRole, MonitorGeometry, PowerEvent, PowerMonitor, SingleInstance,
    configure_overlay_window, native_monitor_geometries,
};
use slint::winit_030::{WinitWindowAccessor, winit};
use slint::{ComponentHandle, SharedString, Timer, TimerMode};
use std::cell::RefCell;
use std::error::Error;
use std::path::{Path, PathBuf};
use std::rc::{Rc, Weak};
use std::time::{Duration, Instant, SystemTime, UNIX_EPOCH};

slint::include_modules!();

type AppResult<T> = Result<T, Box<dyn Error>>;

struct SystemClock {
    monotonic_origin: Instant,
}

impl SystemClock {
    fn new() -> Self {
        Self {
            monotonic_origin: Instant::now(),
        }
    }

    fn now(&self) -> TimePoint {
        self.at(Instant::now(), SystemTime::now())
    }

    fn at(&self, monotonic: Instant, wall: SystemTime) -> TimePoint {
        let wall_seconds = wall
            .duration_since(UNIX_EPOCH)
            .unwrap_or_default()
            .as_secs()
            .min(i64::MAX as u64) as i64;
        let monotonic_seconds = monotonic
            .checked_duration_since(self.monotonic_origin)
            .unwrap_or_default()
            .as_secs();
        TimePoint::new(wall_seconds, monotonic_seconds)
    }
}

struct Runtime {
    controller: Controller,
    config: FileConfigStore,
    clock: SystemClock,
    instance: SingleInstance,
    power: PowerMonitor,
    main: MainWindow,
    settings: Option<SettingsWindow>,
    tray: AppTray,
    break_windows: Vec<BreakOverlay>,
    unlock_answer: i32,
    resource_test: bool,
    resource_state_path: Option<PathBuf>,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
struct MonitorKey {
    x: i32,
    y: i32,
    width: u32,
    height: u32,
    scale_bits: u64,
}

struct MonitorSpec {
    key: MonitorKey,
}

struct BreakOverlay {
    key: MonitorKey,
    window: BreakWindow,
}

const FALLBACK_MONITOR_KEY: MonitorKey = MonitorKey {
    x: 0,
    y: 0,
    width: 0,
    height: 0,
    scale_bits: 0,
};

fn main() -> AppResult<()> {
    slint::BackendSelector::new()
        .backend_name("winit".into())
        .renderer_name("software".into())
        .select()?;

    let arguments: Vec<_> = std::env::args_os().collect();
    let resource_test = arguments
        .iter()
        .any(|argument| argument == "--resource-test" || argument == "--resource-test-break");
    let start_in_break = arguments
        .iter()
        .any(|argument| argument == "--resource-test-break");
    let resource_test_exit_after = arguments.iter().find_map(|argument| {
        argument
            .to_str()
            .and_then(|argument| argument.strip_prefix("--resource-test-seconds="))
            .and_then(|seconds| seconds.parse::<u64>().ok())
            .map(|seconds| Duration::from_secs(seconds.clamp(1, 300)))
    });
    let resource_state_path = resource_test
        .then(|| std::env::var_os("KT_ALARM_CLOCK_RESOURCE_STATE_PATH"))
        .flatten()
        .map(PathBuf::from);
    let instance = match SingleInstance::acquire(&clock_platform::instance_lock_path()?)? {
        InstanceRole::Primary(instance) => instance,
        InstanceRole::Secondary => return Ok(()),
    };

    let config = FileConfigStore::new(clock_platform::config_path()?);
    let stored = config.load()?;
    let initial_settings = match stored {
        Some(settings) => settings,
        None => clock_platform::load_legacy_settings()?.unwrap_or_default(),
    };
    if stored.is_none() {
        config.save(initial_settings)?;
    }
    if !resource_test {
        clock_platform::sync_auto_start(initial_settings.auto_start)?;
    }

    let runtime = Rc::new(RefCell::new(Runtime {
        controller: Controller::new(initial_settings)?,
        config,
        clock: SystemClock::new(),
        instance,
        power: PowerMonitor::new()?,
        main: MainWindow::new()?,
        settings: None,
        tray: AppTray::new()?,
        break_windows: Vec::new(),
        unlock_answer: 0,
        resource_test,
        resource_state_path,
    }));

    install_callbacks(&runtime);
    refresh_views(&runtime);

    runtime.borrow().main.show()?;
    runtime.borrow().tray.show()?;
    write_resource_state(&runtime)?;
    dispatch(&runtime, Event::User(UserAction::PlayPause));
    if start_in_break {
        let weak_runtime = Rc::downgrade(&runtime);
        let weak_main = runtime.borrow().main.as_weak();
        slint::spawn_local(async move {
            let Some(main) = weak_main.upgrade() else {
                return;
            };
            match main.window().winit_window().await {
                Ok(_native) => {
                    if let Some(runtime) = weak_runtime.upgrade() {
                        dispatch(&runtime, Event::User(UserAction::StartBreak));
                    }
                }
                Err(error) => {
                    eprintln!("资源测试等待主窗口原生句柄失败: {error}");
                }
            }
        })?;
    }

    let timer = Timer::default();
    let weak_runtime = Rc::downgrade(&runtime);
    timer.start(TimerMode::Repeated, Duration::from_secs(1), move || {
        if let Some(runtime) = weak_runtime.upgrade() {
            poll_platform_events(&runtime);
            dispatch(&runtime, Event::Tick);
            if let Err(error) = reconcile_break_windows(&runtime) {
                eprintln!("同步多屏休息遮罩失败: {error}");
            }
        }
    });

    if let Some(duration) = resource_test_exit_after {
        Timer::single_shot(duration, || {
            if let Err(error) = slint::quit_event_loop() {
                eprintln!("结束资源测试事件循环失败: {error}");
            }
        });
    }

    slint::run_event_loop()?;
    Ok(())
}

fn install_callbacks(runtime: &Rc<RefCell<Runtime>>) {
    let weak = Rc::downgrade(runtime);
    runtime.borrow().main.on_toggle_play(move || {
        dispatch_weak(&weak, Event::User(UserAction::PlayPause));
    });

    let weak = Rc::downgrade(runtime);
    runtime.borrow().main.on_start_break(move || {
        dispatch_weak(&weak, Event::User(UserAction::StartBreak));
    });

    let weak = Rc::downgrade(runtime);
    runtime.borrow().main.on_adjust_work(move |seconds| {
        dispatch_weak(&weak, Event::User(UserAction::AdjustWork(seconds)));
    });

    let weak = Rc::downgrade(runtime);
    runtime.borrow().main.on_show_settings(move || {
        dispatch_weak(&weak, Event::User(UserAction::ShowSettings));
    });

    let weak = Rc::downgrade(runtime);
    runtime.borrow().main.on_drag_requested(move || {
        if let Some(runtime) = weak.upgrade() {
            let _ = runtime
                .borrow()
                .main
                .window()
                .with_winit_window(|window| window.drag_window());
        }
    });

    let weak = Rc::downgrade(runtime);
    runtime.borrow().tray.on_toggle_play(move || {
        dispatch_weak(&weak, Event::User(UserAction::PlayPause));
    });

    let weak = Rc::downgrade(runtime);
    runtime.borrow().tray.on_start_break(move || {
        dispatch_weak(&weak, Event::User(UserAction::StartBreak));
    });

    let weak = Rc::downgrade(runtime);
    runtime.borrow().tray.on_show_main(move || {
        if let Some(runtime) = weak.upgrade() {
            show_and_activate_main(&runtime);
        }
    });

    let weak = Rc::downgrade(runtime);
    runtime.borrow().tray.on_show_settings(move || {
        dispatch_weak(&weak, Event::User(UserAction::ShowSettings));
    });

    let weak = Rc::downgrade(runtime);
    runtime.borrow().tray.on_quit(move || {
        dispatch_weak(&weak, Event::User(UserAction::Quit));
    });
}

fn dispatch_weak(runtime: &Weak<RefCell<Runtime>>, event: Event) {
    if let Some(runtime) = runtime.upgrade() {
        dispatch(&runtime, event);
    }
}

fn dispatch(runtime: &Rc<RefCell<Runtime>>, event: Event) {
    let now = runtime.borrow().clock.now();
    dispatch_at(runtime, event, now);
}

fn dispatch_at(runtime: &Rc<RefCell<Runtime>>, event: Event, now: TimePoint) {
    let result = {
        let mut runtime = runtime.borrow_mut();
        runtime.controller.dispatch(event, now)
    };

    match result {
        Ok(effects) => {
            for effect in effects {
                if let Err(error) = apply_effect(runtime, effect) {
                    eprintln!("执行应用副作用失败: {error}");
                }
            }
            refresh_views(runtime);
        }
        Err(error) => {
            set_settings_error(runtime, SharedString::from(error.to_string()));
        }
    }
}

fn apply_effect(runtime: &Rc<RefCell<Runtime>>, effect: Effect) -> AppResult<()> {
    match effect {
        Effect::ShowMain => runtime.borrow().main.show()?,
        Effect::HideMain => runtime.borrow().main.hide()?,
        Effect::ShowSettings => {
            ensure_settings_window(runtime)?;
            let runtime = runtime.borrow();
            let settings = runtime.settings.as_ref().expect("设置窗口刚完成创建");
            set_settings_view(settings, runtime.controller.settings());
            settings.set_error_text(SharedString::default());
            settings.show()?;
        }
        Effect::ShowBreakOverlays => show_break_windows(runtime)?,
        Effect::DismissBreakOverlays => dismiss_break_windows(runtime)?,
        Effect::ActivateMain => show_and_activate_main(runtime),
        Effect::PersistSettings(settings) => runtime.borrow().config.save(settings)?,
        Effect::SetAutoStart(enabled) if !runtime.borrow().resource_test => {
            clock_platform::sync_auto_start(enabled)?;
        }
        Effect::SetAutoStart(_) => {}
        Effect::Exit => slint::quit_event_loop()?,
    }
    Ok(())
}

fn show_break_windows(runtime: &Rc<RefCell<Runtime>>) -> AppResult<()> {
    if !runtime.borrow().break_windows.is_empty() {
        return Ok(());
    }

    let overlays = build_break_windows(runtime)?;
    runtime.borrow_mut().break_windows = overlays;
    write_resource_state(runtime)?;
    Ok(())
}

fn build_break_windows(runtime: &Rc<RefCell<Runtime>>) -> AppResult<Vec<BreakOverlay>> {
    let mut monitors = monitor_specs(runtime);
    monitors.sort_by_key(|monitor| monitor.key);

    let now = runtime.borrow().clock.now();
    let a = (now.wall_seconds.rem_euclid(900) + 100) as i32;
    let b = ((now.wall_seconds / 7).rem_euclid(900) + 100) as i32;
    runtime.borrow_mut().unlock_answer = a + b;

    if monitors.is_empty() {
        let window = create_break_window(runtime, a, b)?;
        window.show()?;
        schedule_native_overlay_configuration(&window, None)?;
        return Ok(vec![BreakOverlay {
            key: FALLBACK_MONITOR_KEY,
            window,
        }]);
    }

    let mut overlays = Vec::with_capacity(monitors.len());
    for monitor in monitors {
        let window = create_break_window(runtime, a, b)?;
        window
            .window()
            .set_position(slint::PhysicalPosition::new(monitor.key.x, monitor.key.y));
        window.window().set_size(slint::PhysicalSize::new(
            monitor.key.width,
            monitor.key.height,
        ));
        window.show()?;
        schedule_native_overlay_configuration(&window, Some(monitor.key))?;
        overlays.push(BreakOverlay {
            key: monitor.key,
            window,
        });
    }
    Ok(overlays)
}

fn schedule_native_overlay_configuration(
    window: &BreakWindow,
    monitor: Option<MonitorKey>,
) -> AppResult<()> {
    let weak = window.as_weak();
    slint::spawn_local(async move {
        let Some(window) = weak.upgrade() else {
            return;
        };
        match window.window().winit_window().await {
            Ok(native) => {
                native.set_window_level(winit::window::WindowLevel::AlwaysOnTop);
                if let Some(monitor) = monitor {
                    native.set_fullscreen(None);
                    native.set_outer_position(winit::dpi::PhysicalPosition::new(
                        monitor.x, monitor.y,
                    ));
                    let _ = native.request_inner_size(winit::dpi::PhysicalSize::new(
                        monitor.width,
                        monitor.height,
                    ));
                } else {
                    native.set_fullscreen(Some(winit::window::Fullscreen::Borderless(None)));
                }
                if let Err(error) = configure_overlay_window(native.as_ref()) {
                    eprintln!("配置原生休息遮罩失败: {error}");
                }
            }
            Err(error) => eprintln!("取得休息遮罩 winit 窗口失败: {error}"),
        }
    })?;
    Ok(())
}

fn create_break_window(runtime: &Rc<RefCell<Runtime>>, a: i32, b: i32) -> AppResult<BreakWindow> {
    let window = BreakWindow::new()?;
    window.set_formula_text(format!("{a} + {b} =").into());

    let weak = Rc::downgrade(runtime);
    window.on_unlock_requested(move |answer| {
        if let Some(runtime) = weak.upgrade() {
            handle_unlock(&runtime, answer.as_str());
        }
    });
    Ok(window)
}

fn monitor_specs(runtime: &Rc<RefCell<Runtime>>) -> Vec<MonitorSpec> {
    let mut monitors: Vec<MonitorSpec> = runtime
        .borrow()
        .main
        .window()
        .with_winit_window(|window| window.available_monitors().map(monitor_spec).collect())
        .unwrap_or_default();
    if monitors.is_empty() {
        monitors = native_monitor_geometries()
            .unwrap_or_else(|error| {
                eprintln!("原生显示器枚举失败: {error}");
                Vec::new()
            })
            .into_iter()
            .map(platform_monitor_spec)
            .collect();
    }
    if monitors.is_empty() {
        monitors = runtime
            .borrow()
            .main
            .window()
            .with_winit_window(|window| window.current_monitor().map(monitor_spec))
            .flatten()
            .into_iter()
            .collect();
    }
    monitors
}

fn monitor_spec(handle: winit::monitor::MonitorHandle) -> MonitorSpec {
    let position = handle.position();
    let size = handle.size();
    MonitorSpec {
        key: MonitorKey {
            x: position.x,
            y: position.y,
            width: size.width,
            height: size.height,
            scale_bits: handle.scale_factor().to_bits(),
        },
    }
}

fn platform_monitor_spec(geometry: MonitorGeometry) -> MonitorSpec {
    MonitorSpec {
        key: MonitorKey {
            x: geometry.x,
            y: geometry.y,
            width: geometry.width,
            height: geometry.height,
            scale_bits: geometry.scale_bits,
        },
    }
}

fn reconcile_break_windows(runtime: &Rc<RefCell<Runtime>>) -> AppResult<()> {
    let in_break = {
        let runtime = runtime.borrow();
        runtime.controller.snapshot(runtime.clock.now()).phase == Phase::Break
    };
    if !in_break {
        return Ok(());
    }

    let expected = expected_monitor_keys(
        monitor_specs(runtime)
            .into_iter()
            .map(|monitor| monitor.key),
    );
    let current = sorted_monitor_keys(
        runtime
            .borrow()
            .break_windows
            .iter()
            .map(|overlay| overlay.key),
    );
    if current == expected {
        return Ok(());
    }

    let replacements = build_break_windows(runtime)?;
    let previous = std::mem::replace(&mut runtime.borrow_mut().break_windows, replacements);
    write_resource_state(runtime)?;
    hide_break_overlays(previous)
}

fn dismiss_break_windows(runtime: &Rc<RefCell<Runtime>>) -> AppResult<()> {
    let overlays = std::mem::take(&mut runtime.borrow_mut().break_windows);
    write_resource_state(runtime)?;
    hide_break_overlays(overlays)
}

fn hide_break_overlays(overlays: Vec<BreakOverlay>) -> AppResult<()> {
    for overlay in overlays {
        overlay.window.hide()?;
    }
    Ok(())
}

fn handle_unlock(runtime: &Rc<RefCell<Runtime>>, answer: &str) {
    let now = runtime.borrow().clock.now();
    let snapshot = runtime.borrow().controller.snapshot(now);
    if !snapshot.can_unlock {
        return;
    }
    if snapshot.remaining_seconds > 0
        && answer.trim().parse::<i32>().ok() != Some(runtime.borrow().unlock_answer)
    {
        for overlay in &runtime.borrow().break_windows {
            overlay.window.set_message_text("答案不正确".into());
        }
        return;
    }
    dispatch(runtime, Event::Unlock);
}

fn apply_settings(
    runtime: &Rc<RefCell<Runtime>>,
    work: i32,
    rest: i32,
    force: i32,
    auto_start: bool,
) {
    let converted = u32::try_from(work)
        .and_then(|work_seconds| {
            Ok(Settings {
                work_seconds,
                break_seconds: u32::try_from(rest)?,
                force_seconds: u32::try_from(force)?,
                auto_start,
                ..runtime.borrow().controller.settings()
            })
        })
        .map_err(|error| error.to_string());

    match converted {
        Ok(settings) => {
            dispatch(runtime, Event::ApplySettings(settings));
            set_settings_error(runtime, SharedString::default());
        }
        Err(error) => set_settings_error(runtime, SharedString::from(error)),
    }
}

fn refresh_views(runtime: &Rc<RefCell<Runtime>>) {
    let runtime = runtime.borrow();
    let snapshot = runtime.controller.snapshot(runtime.clock.now());
    let time_text = format_seconds(snapshot.remaining_seconds);

    runtime.main.set_time_text(time_text.clone().into());
    runtime.main.set_running(snapshot.running);
    runtime.main.set_in_break(snapshot.phase == Phase::Break);

    if let Some(settings) = &runtime.settings {
        settings.set_running(snapshot.running);
    }

    runtime.tray.set_running(snapshot.running);
    runtime.tray.set_in_break(snapshot.phase == Phase::Break);
    runtime
        .tray
        .set_status_text(status_text(snapshot, &time_text).into());

    for overlay in &runtime.break_windows {
        let window = &overlay.window;
        window.set_time_text(time_text.clone().into());
        window.set_forced(snapshot.forced);
        window.set_can_unlock(snapshot.can_unlock);
        window.set_break_finished(snapshot.remaining_seconds == 0);
        if !snapshot.can_unlock {
            window.set_message_text(SharedString::default());
        }
    }
}

fn set_settings_view(window: &SettingsWindow, settings: Settings) {
    window.set_work_seconds(settings.work_seconds as i32);
    window.set_break_seconds(settings.break_seconds as i32);
    window.set_force_seconds(settings.force_seconds as i32);
    window.set_auto_start(settings.auto_start);
}

fn ensure_settings_window(runtime: &Rc<RefCell<Runtime>>) -> AppResult<()> {
    if runtime.borrow().settings.is_some() {
        return Ok(());
    }

    let window = SettingsWindow::new()?;
    install_settings_callbacks(runtime, &window);
    runtime.borrow_mut().settings = Some(window);
    Ok(())
}

fn install_settings_callbacks(runtime: &Rc<RefCell<Runtime>>, window: &SettingsWindow) {
    let weak = Rc::downgrade(runtime);
    window.on_toggle_play(move || {
        dispatch_weak(&weak, Event::User(UserAction::PlayPause));
    });

    let weak = Rc::downgrade(runtime);
    window.on_start_break(move || {
        dispatch_weak(&weak, Event::User(UserAction::StartBreak));
    });

    let weak = Rc::downgrade(runtime);
    window.on_next_work(move || {
        dispatch_weak(&weak, Event::User(UserAction::NextWork));
    });

    let weak = Rc::downgrade(runtime);
    window.on_adjust_work(move |seconds| {
        dispatch_weak(&weak, Event::User(UserAction::AdjustWork(seconds)));
    });

    let weak = Rc::downgrade(runtime);
    window.on_save_settings(move |work, rest, force, auto_start| {
        if let Some(runtime) = weak.upgrade() {
            apply_settings(&runtime, work, rest, force, auto_start);
        }
    });

    let weak = Rc::downgrade(runtime);
    window.on_restore_defaults(move || {
        if let Some(runtime) = weak.upgrade() {
            let defaults = Settings::default();
            {
                let runtime = runtime.borrow();
                if let Some(settings) = &runtime.settings {
                    set_settings_view(settings, defaults);
                }
            }
            dispatch(&runtime, Event::ApplySettings(defaults));
        }
    });
}

fn set_settings_error(runtime: &Rc<RefCell<Runtime>>, error: SharedString) {
    if let Some(settings) = &runtime.borrow().settings {
        settings.set_error_text(error);
    } else if !error.is_empty() {
        eprintln!("设置操作失败: {error}");
    }
}

fn show_and_activate_main(runtime: &Rc<RefCell<Runtime>>) {
    let runtime = runtime.borrow();
    if let Err(error) = runtime.main.show() {
        eprintln!("显示主窗口失败: {error}");
        return;
    }
    let _ = runtime
        .main
        .window()
        .with_winit_window(|window| window.focus_window());
}

fn poll_platform_events(runtime: &Rc<RefCell<Runtime>>) {
    let power_events = runtime.borrow().power.drain();
    for event in power_events {
        let (event, now) = match event {
            PowerEvent::Suspended { monotonic, wall } => {
                (Event::Suspended, runtime.borrow().clock.at(monotonic, wall))
            }
            PowerEvent::Resumed { monotonic, wall } => {
                (Event::Resumed, runtime.borrow().clock.at(monotonic, wall))
            }
        };
        dispatch_at(runtime, event, now);
    }

    let activation = runtime.borrow().instance.take_activation_request();
    match activation {
        Ok(true) => dispatch(runtime, Event::SecondInstance),
        Ok(false) => {}
        Err(error) => eprintln!("读取第二实例激活请求失败: {error}"),
    }
}

fn status_text(snapshot: Snapshot, time_text: &str) -> String {
    match snapshot.phase {
        Phase::Break => format!("休息 {time_text}"),
        Phase::Work if snapshot.running => format!("工作 {time_text}"),
        Phase::Work | Phase::Idle => format!("已暂停 {time_text}"),
    }
}

fn format_seconds(seconds: u32) -> String {
    format!("{}:{:02}", seconds / 60, seconds % 60)
}

fn sorted_monitor_keys(keys: impl IntoIterator<Item = MonitorKey>) -> Vec<MonitorKey> {
    let mut keys: Vec<_> = keys.into_iter().collect();
    keys.sort_unstable();
    keys
}

fn expected_monitor_keys(keys: impl IntoIterator<Item = MonitorKey>) -> Vec<MonitorKey> {
    let mut keys = sorted_monitor_keys(keys);
    if keys.is_empty() {
        keys.push(FALLBACK_MONITOR_KEY);
    }
    keys
}

fn write_resource_state(runtime: &Rc<RefCell<Runtime>>) -> AppResult<()> {
    let runtime = runtime.borrow();
    let Some(path) = &runtime.resource_state_path else {
        return Ok(());
    };
    if let Some(parent) = path.parent() {
        std::fs::create_dir_all(parent)?;
    }
    let overlay_count = runtime.break_windows.len();
    let overlay_pixels: u64 = runtime
        .break_windows
        .iter()
        .map(|overlay| u64::from(overlay.key.width) * u64::from(overlay.key.height))
        .sum();
    let contents = format!("overlay_count={overlay_count}\noverlay_pixels={overlay_pixels}\n");
    let temporary = sibling_temporary_path(path);
    std::fs::write(&temporary, contents)?;
    if path.exists() {
        std::fs::remove_file(path)?;
    }
    std::fs::rename(temporary, path)?;
    Ok(())
}

fn sibling_temporary_path(path: &Path) -> PathBuf {
    let name = path
        .file_name()
        .and_then(|name| name.to_str())
        .unwrap_or("resource-state");
    path.with_file_name(format!(".{name}.tmp"))
}

#[cfg(test)]
mod tests {
    use super::*;

    fn monitor(x: i32, scale: f64) -> MonitorKey {
        MonitorKey {
            x,
            y: 0,
            width: 1_920,
            height: 1_080,
            scale_bits: scale.to_bits(),
        }
    }

    #[test]
    fn monitor_comparison_is_independent_of_enumeration_order() {
        assert_eq!(
            expected_monitor_keys([monitor(1_920, 1.5), monitor(0, 1.0)]),
            sorted_monitor_keys([monitor(0, 1.0), monitor(1_920, 1.5)])
        );
    }

    #[test]
    fn monitor_comparison_detects_geometry_and_scale_changes() {
        let original = expected_monitor_keys([monitor(0, 1.0)]);
        assert_ne!(original, expected_monitor_keys([monitor(100, 1.0)]));
        assert_ne!(original, expected_monitor_keys([monitor(0, 2.0)]));
    }

    #[test]
    fn missing_monitor_enumeration_uses_one_fullscreen_fallback() {
        assert_eq!(
            expected_monitor_keys(std::iter::empty()),
            vec![FALLBACK_MONITOR_KEY]
        );
        assert!(sorted_monitor_keys(std::iter::empty()).is_empty());
    }

    #[test]
    fn seconds_are_formatted_without_subsecond_work() {
        assert_eq!(format_seconds(0), "0:00");
        assert_eq!(format_seconds(61), "1:01");
        assert_eq!(format_seconds(3_661), "61:01");
    }

    #[test]
    fn queued_power_events_keep_their_original_timestamps() {
        let monotonic_origin = Instant::now();
        let clock = SystemClock { monotonic_origin };
        let event_time = clock.at(
            monotonic_origin + Duration::from_secs(37),
            UNIX_EPOCH + Duration::from_secs(12_345),
        );

        assert_eq!(event_time, TimePoint::new(12_345, 37));
    }
}
