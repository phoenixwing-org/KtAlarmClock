mod autostart;
mod instance;
mod monitor;
mod overlay;
mod power;
mod resource;

use clock_domain::Settings;
use std::error::Error;
use std::path::PathBuf;

pub use instance::{InstanceRole, SingleInstance};
pub use monitor::{MonitorGeometry, native_monitor_geometries};
pub use overlay::configure_overlay_window;
pub use power::{PowerEvent, PowerMonitor};
pub use resource::{ProcessSample, sample_process};

pub type PlatformResult<T> = Result<T, Box<dyn Error>>;

pub fn config_path() -> PlatformResult<PathBuf> {
    Ok(platform_config_dir()?.join("config.toml"))
}

pub fn instance_lock_path() -> PlatformResult<PathBuf> {
    Ok(platform_config_dir()?.join("instance.lock"))
}

pub fn sync_auto_start(enabled: bool) -> PlatformResult<()> {
    autostart::sync(enabled)
}

pub fn load_legacy_settings() -> PlatformResult<Option<Settings>> {
    autostart::load_legacy_settings()
}

fn configured_data_dir() -> Option<PathBuf> {
    std::env::var_os("KT_ALARM_CLOCK_DATA_DIR")
        .filter(|value| !value.is_empty())
        .map(PathBuf::from)
}

#[cfg(target_os = "windows")]
fn platform_config_dir() -> PlatformResult<PathBuf> {
    if let Some(path) = configured_data_dir() {
        return Ok(path);
    }
    let root = std::env::var_os("APPDATA").ok_or("找不到 APPDATA 用户配置目录")?;
    Ok(PathBuf::from(root).join("KuntaiSoft").join("KtAlarmClock"))
}

#[cfg(target_os = "macos")]
fn platform_config_dir() -> PlatformResult<PathBuf> {
    if let Some(path) = configured_data_dir() {
        return Ok(path);
    }
    let root = std::env::var_os("HOME").ok_or("找不到 HOME 用户目录")?;
    Ok(PathBuf::from(root)
        .join("Library")
        .join("Application Support")
        .join("KuntaiSoft")
        .join("KtAlarmClock"))
}

#[cfg(all(unix, not(target_os = "macos")))]
fn platform_config_dir() -> PlatformResult<PathBuf> {
    if let Some(path) = configured_data_dir() {
        return Ok(path);
    }
    let root = std::env::var_os("XDG_CONFIG_HOME")
        .map(PathBuf::from)
        .or_else(|| std::env::var_os("HOME").map(|home| PathBuf::from(home).join(".config")))
        .ok_or("找不到 XDG_CONFIG_HOME/HOME 用户配置目录")?;
    Ok(root.join("KtAlarmClock"))
}
