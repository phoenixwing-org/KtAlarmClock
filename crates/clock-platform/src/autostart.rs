use crate::PlatformResult;
use clock_domain::Settings;

pub(crate) fn sync(enabled: bool) -> PlatformResult<()> {
    imp::sync(enabled)
}

pub(crate) fn load_legacy_settings() -> PlatformResult<Option<Settings>> {
    imp::load_legacy_settings()
}

#[cfg(target_os = "macos")]
mod imp {
    use super::*;
    use std::fs;
    use std::io::Write;
    use std::path::{Path, PathBuf};

    const LABEL: &str = "com.kuntaisoft.KtAlarmClock";

    pub(super) fn sync(enabled: bool) -> PlatformResult<()> {
        let path = launch_agent_path()?;
        if !enabled {
            if path.exists() {
                fs::remove_file(path)?;
            }
            return Ok(());
        }

        let executable = std::env::current_exe()?;
        let expected = launch_agent_contents(&executable);
        if fs::read_to_string(&path).ok().as_deref() == Some(expected.as_str()) {
            return Ok(());
        }

        if let Some(parent) = path.parent() {
            fs::create_dir_all(parent)?;
        }
        atomic_write(&path, expected.as_bytes())
    }

    pub(super) fn load_legacy_settings() -> PlatformResult<Option<Settings>> {
        Ok(None)
    }

    fn launch_agent_path() -> PlatformResult<PathBuf> {
        let home = std::env::var_os("HOME").ok_or("找不到 HOME 用户目录")?;
        Ok(PathBuf::from(home)
            .join("Library")
            .join("LaunchAgents")
            .join(format!("{LABEL}.plist")))
    }

    fn launch_agent_contents(executable: &Path) -> String {
        let executable = xml_escape(&executable.to_string_lossy());
        format!(
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
             <!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \
             \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n\
             <plist version=\"1.0\">\n\
             <dict>\n\
             \t<key>Label</key>\n\
             \t<string>{LABEL}</string>\n\
             \t<key>ProgramArguments</key>\n\
             \t<array>\n\
             \t\t<string>{executable}</string>\n\
             \t</array>\n\
             \t<key>RunAtLoad</key>\n\
             \t<true/>\n\
             </dict>\n\
             </plist>\n"
        )
    }

    fn xml_escape(value: &str) -> String {
        value
            .replace('&', "&amp;")
            .replace('<', "&lt;")
            .replace('>', "&gt;")
            .replace('"', "&quot;")
            .replace('\'', "&apos;")
    }

    fn atomic_write(path: &Path, contents: &[u8]) -> PlatformResult<()> {
        let temporary = path.with_extension(format!("tmp-{}", std::process::id()));
        let mut file = fs::File::create(&temporary)?;
        file.write_all(contents)?;
        file.sync_all()?;
        fs::rename(temporary, path)?;
        Ok(())
    }

    #[cfg(test)]
    mod tests {
        use super::*;

        #[test]
        fn launch_agent_escapes_executable_path() {
            let plist = launch_agent_contents(Path::new("/Applications/A&B<Clock>.app"));
            assert!(plist.contains("/Applications/A&amp;B&lt;Clock&gt;.app"));
            assert!(plist.contains("<key>RunAtLoad</key>"));
        }
    }
}

#[cfg(target_os = "windows")]
#[allow(unsafe_code)]
mod imp {
    use super::*;
    use std::mem::size_of;
    use std::path::Path;
    use windows::Win32::Foundation::{ERROR_FILE_NOT_FOUND, ERROR_SUCCESS};
    use windows::Win32::System::Registry::{
        HKEY, HKEY_CURRENT_USER, KEY_READ, KEY_SET_VALUE, REG_OPTION_NON_VOLATILE, REG_SZ,
        RRF_RT_REG_DWORD, RRF_RT_REG_SZ, RegCloseKey, RegCreateKeyExW, RegDeleteValueW,
        RegGetValueW, RegSetValueExW,
    };
    use windows::core::{PCWSTR, w};

    const RUN_KEY: PCWSTR = w!("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
    const LEGACY_KEY: PCWSTR = w!("SOFTWARE\\KuntaiSoft\\KtAlarmClock");
    const VALUE_NAME: PCWSTR = w!("KtAlarmClock");

    pub(super) fn sync(enabled: bool) -> PlatformResult<()> {
        let key = RegistryKey::create(HKEY_CURRENT_USER, RUN_KEY)?;
        if !enabled {
            let status = unsafe { RegDeleteValueW(key.0, VALUE_NAME) };
            if status != ERROR_SUCCESS && status != ERROR_FILE_NOT_FOUND {
                status.ok()?;
            }
            return Ok(());
        }

        let executable = std::env::current_exe()?;
        let desired = quoted_executable(&executable);
        if read_string(key.0, VALUE_NAME)?.as_deref() == Some(desired.as_str()) {
            return Ok(());
        }

        let wide = wide_null(&desired);
        let bytes = unsafe {
            std::slice::from_raw_parts(wide.as_ptr().cast::<u8>(), wide.len() * size_of::<u16>())
        };
        unsafe { RegSetValueExW(key.0, VALUE_NAME, None, REG_SZ, Some(bytes)) }.ok()?;
        Ok(())
    }

    pub(super) fn load_legacy_settings() -> PlatformResult<Option<Settings>> {
        let work = read_dword(HKEY_CURRENT_USER, LEGACY_KEY, w!("WorkTime"))?;
        let Some(work_seconds) = work else {
            return Ok(None);
        };

        let defaults = Settings::default();
        let settings = Settings {
            work_seconds,
            break_seconds: read_dword(HKEY_CURRENT_USER, LEGACY_KEY, w!("WorkBreak"))?
                .unwrap_or(defaults.break_seconds),
            force_seconds: read_dword(HKEY_CURRENT_USER, LEGACY_KEY, w!("TimeForce"))?
                .unwrap_or(defaults.force_seconds),
            ..defaults
        };
        Ok(settings.validate().ok().map(|_| settings))
    }

    struct RegistryKey(HKEY);

    impl RegistryKey {
        fn create(root: HKEY, path: PCWSTR) -> PlatformResult<Self> {
            let mut key = HKEY::default();
            unsafe {
                RegCreateKeyExW(
                    root,
                    path,
                    None,
                    PCWSTR::null(),
                    REG_OPTION_NON_VOLATILE,
                    KEY_READ | KEY_SET_VALUE,
                    None,
                    &mut key,
                    None,
                )
            }
            .ok()?;
            Ok(Self(key))
        }
    }

    impl Drop for RegistryKey {
        fn drop(&mut self) {
            let _ = unsafe { RegCloseKey(self.0) };
        }
    }

    fn read_dword(root: HKEY, key: PCWSTR, value: PCWSTR) -> PlatformResult<Option<u32>> {
        let mut result = 0_u32;
        let mut size = size_of::<u32>() as u32;
        let status = unsafe {
            RegGetValueW(
                root,
                key,
                value,
                RRF_RT_REG_DWORD,
                None,
                Some((&mut result as *mut u32).cast()),
                Some(&mut size),
            )
        };
        if status == ERROR_FILE_NOT_FOUND {
            return Ok(None);
        }
        status.ok()?;
        Ok(Some(result))
    }

    fn read_string(key: HKEY, value: PCWSTR) -> PlatformResult<Option<String>> {
        let mut byte_count = 0_u32;
        let status = unsafe {
            RegGetValueW(
                key,
                PCWSTR::null(),
                value,
                RRF_RT_REG_SZ,
                None,
                None,
                Some(&mut byte_count),
            )
        };
        if status == ERROR_FILE_NOT_FOUND {
            return Ok(None);
        }
        status.ok()?;

        let mut wide = vec![0_u16; (byte_count as usize).div_ceil(size_of::<u16>())];
        unsafe {
            RegGetValueW(
                key,
                PCWSTR::null(),
                value,
                RRF_RT_REG_SZ,
                None,
                Some(wide.as_mut_ptr().cast()),
                Some(&mut byte_count),
            )
        }
        .ok()?;
        if wide.last() == Some(&0) {
            wide.pop();
        }
        Ok(Some(String::from_utf16(&wide)?))
    }

    fn quoted_executable(executable: &Path) -> String {
        format!("\"{}\"", executable.display())
    }

    fn wide_null(value: &str) -> Vec<u16> {
        value.encode_utf16().chain(std::iter::once(0)).collect()
    }
}

#[cfg(not(any(target_os = "macos", target_os = "windows")))]
mod imp {
    use super::*;

    pub(super) fn sync(_enabled: bool) -> PlatformResult<()> {
        Ok(())
    }

    pub(super) fn load_legacy_settings() -> PlatformResult<Option<Settings>> {
        Ok(None)
    }
}
