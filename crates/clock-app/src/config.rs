use clock_domain::Settings;
use serde::{Deserialize, Serialize};
use std::error::Error;
use std::fmt::{Display, Formatter};
use std::fs::{self, File, OpenOptions};
use std::io::{self, Write};
use std::path::{Path, PathBuf};

#[derive(Debug, Clone)]
pub struct FileConfigStore {
    path: PathBuf,
}

impl FileConfigStore {
    pub fn new(path: impl Into<PathBuf>) -> Self {
        Self { path: path.into() }
    }

    pub fn path(&self) -> &Path {
        &self.path
    }

    pub fn load(&self) -> Result<Option<Settings>, ConfigError> {
        let Some(file) = self.load_file_settings()? else {
            return Ok(None);
        };
        let settings = Settings::from(file);
        settings.validate().map(Some).map_err(ConfigError::Invalid)
    }

    pub fn load_main_window_position(&self) -> Result<Option<MainWindowPosition>, ConfigError> {
        Ok(self
            .load_file_settings()?
            .and_then(|file| file.main_window_position()))
    }

    pub fn load_or_default(&self) -> Result<Settings, ConfigError> {
        Ok(self.load()?.unwrap_or_default())
    }

    pub fn save(&self, settings: Settings) -> Result<(), ConfigError> {
        let position = self.load_main_window_position()?;
        self.save_with_position(settings, position)
    }

    pub fn save_with_position(
        &self,
        settings: Settings,
        main_window_position: Option<MainWindowPosition>,
    ) -> Result<(), ConfigError> {
        let settings = settings.validate().map_err(ConfigError::Invalid)?;
        if let Some(parent) = self.path.parent() {
            fs::create_dir_all(parent).map_err(ConfigError::Io)?;
        }
        let mut file = FileSettings::from(settings);
        file.set_main_window_position(main_window_position);
        let encoded = toml::to_string_pretty(&file).map_err(ConfigError::Serialize)?;
        let temp = sibling_path(&self.path, "tmp");
        let backup = sibling_path(&self.path, "bak");

        let mut file = create_fresh(&temp)?;
        if let Err(error) = write_and_sync(&mut file, encoded.as_bytes()) {
            let _ = fs::remove_file(&temp);
            return Err(ConfigError::Io(error));
        }
        drop(file);

        // Unix rename 会原子替换；Windows 先保留可恢复备份，平台层后续使用
        // MoveFileExW(REPLACE_EXISTING) 收紧为真正原子替换。
        if self.path.exists() {
            let _ = fs::remove_file(&backup);
            fs::rename(&self.path, &backup).map_err(ConfigError::Io)?;
        }
        if let Err(error) = fs::rename(&temp, &self.path) {
            let _ = fs::rename(&backup, &self.path);
            let _ = fs::remove_file(&temp);
            return Err(ConfigError::Io(error));
        }
        let _ = fs::remove_file(&backup);
        sync_parent_directory(&self.path)?;
        Ok(())
    }

    fn load_file_settings(&self) -> Result<Option<FileSettings>, ConfigError> {
        if !self.path.exists() {
            self.restore_backup_if_needed()?;
        }
        if !self.path.exists() {
            return Ok(None);
        }
        let source = fs::read_to_string(&self.path).map_err(ConfigError::Io)?;
        let file: FileSettings = toml::from_str(&source).map_err(ConfigError::Parse)?;
        Ok(Some(file))
    }

    fn restore_backup_if_needed(&self) -> Result<(), ConfigError> {
        let backup = sibling_path(&self.path, "bak");
        if backup.exists() {
            fs::rename(backup, &self.path).map_err(ConfigError::Io)?;
        }
        Ok(())
    }
}

#[derive(Debug)]
pub enum ConfigError {
    Io(io::Error),
    Parse(toml::de::Error),
    Serialize(toml::ser::Error),
    Invalid(clock_domain::SettingsError),
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub struct MainWindowPosition {
    pub x: i32,
    pub y: i32,
}

impl Display for ConfigError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::Io(error) => write!(f, "配置文件 I/O 失败: {error}"),
            Self::Parse(error) => write!(f, "配置文件 TOML 解析失败: {error}"),
            Self::Serialize(error) => write!(f, "配置文件 TOML 序列化失败: {error}"),
            Self::Invalid(error) => write!(f, "配置值无效: {error}"),
        }
    }
}

impl Error for ConfigError {
    fn source(&self) -> Option<&(dyn Error + 'static)> {
        match self {
            Self::Io(error) => Some(error),
            Self::Parse(error) => Some(error),
            Self::Serialize(error) => Some(error),
            Self::Invalid(error) => Some(error),
        }
    }
}

#[derive(Debug, Clone, Copy, Serialize, Deserialize)]
#[serde(deny_unknown_fields)]
struct FileSettings {
    schema_version: u32,
    work_seconds: u32,
    break_seconds: u32,
    force_seconds: u32,
    auto_start: bool,
    #[serde(default)]
    main_window_x: Option<i32>,
    #[serde(default)]
    main_window_y: Option<i32>,
}

impl From<FileSettings> for Settings {
    fn from(value: FileSettings) -> Self {
        Self {
            schema_version: value.schema_version,
            work_seconds: value.work_seconds,
            break_seconds: value.break_seconds,
            force_seconds: value.force_seconds,
            auto_start: value.auto_start,
        }
    }
}

impl FileSettings {
    fn main_window_position(&self) -> Option<MainWindowPosition> {
        self.main_window_x
            .zip(self.main_window_y)
            .map(|(x, y)| MainWindowPosition { x, y })
    }

    fn set_main_window_position(&mut self, position: Option<MainWindowPosition>) {
        match position {
            Some(position) => {
                self.main_window_x = Some(position.x);
                self.main_window_y = Some(position.y);
            }
            None => {
                self.main_window_x = None;
                self.main_window_y = None;
            }
        }
    }
}

impl From<Settings> for FileSettings {
    fn from(value: Settings) -> Self {
        Self {
            schema_version: value.schema_version,
            work_seconds: value.work_seconds,
            break_seconds: value.break_seconds,
            force_seconds: value.force_seconds,
            auto_start: value.auto_start,
            main_window_x: None,
            main_window_y: None,
        }
    }
}

fn sibling_path(path: &Path, extension: &str) -> PathBuf {
    let file_name = path
        .file_name()
        .and_then(|name| name.to_str())
        .unwrap_or("config.toml");
    path.with_file_name(format!(".{file_name}.{extension}"))
}

fn create_fresh(path: &Path) -> Result<File, ConfigError> {
    let _ = fs::remove_file(path);
    OpenOptions::new()
        .write(true)
        .create_new(true)
        .open(path)
        .map_err(ConfigError::Io)
}

fn write_and_sync(file: &mut File, bytes: &[u8]) -> io::Result<()> {
    file.write_all(bytes)?;
    file.sync_all()
}

#[cfg(unix)]
fn sync_parent_directory(path: &Path) -> Result<(), ConfigError> {
    if let Some(parent) = path.parent() {
        File::open(parent)
            .and_then(|directory| directory.sync_all())
            .map_err(ConfigError::Io)?;
    }
    Ok(())
}

#[cfg(not(unix))]
fn sync_parent_directory(_path: &Path) -> Result<(), ConfigError> {
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    #[test]
    fn missing_file_returns_defaults() {
        let directory = tempdir().unwrap();
        let store = FileConfigStore::new(directory.path().join("config.toml"));
        assert_eq!(store.load_or_default().unwrap(), Settings::default());
    }

    #[test]
    fn round_trip_and_update() {
        let directory = tempdir().unwrap();
        let path = directory.path().join("nested/config.toml");
        let store = FileConfigStore::new(&path);
        let first = Settings::default();
        store.save(first).unwrap();
        assert_eq!(store.load().unwrap(), Some(first));
        assert!(store.load_main_window_position().unwrap().is_none());

        let second = Settings {
            auto_start: false,
            work_seconds: 1_200,
            ..first
        };
        let second_position = MainWindowPosition { x: 120, y: 80 };
        store
            .save_with_position(second, Some(second_position))
            .unwrap();
        assert_eq!(store.load().unwrap(), Some(second));
        assert_eq!(
            store.load_main_window_position().unwrap(),
            Some(second_position)
        );
        assert!(!sibling_path(&path, "tmp").exists());
        assert!(!sibling_path(&path, "bak").exists());

        store.save_with_position(second, None).unwrap();
        assert!(store.load_main_window_position().unwrap().is_none());
    }

    #[test]
    fn invalid_values_are_rejected() {
        let directory = tempdir().unwrap();
        let store = FileConfigStore::new(directory.path().join("config.toml"));
        let invalid = Settings {
            force_seconds: 601,
            ..Settings::default()
        };
        assert!(matches!(store.save(invalid), Err(ConfigError::Invalid(_))));
    }

    #[test]
    fn unknown_fields_are_rejected() {
        let directory = tempdir().unwrap();
        let path = directory.path().join("config.toml");
        fs::write(
            &path,
            "schema_version=1\nwork_seconds=2700\nbreak_seconds=600\nforce_seconds=300\nauto_start=true\nextra=1\n",
        )
        .unwrap();
        let store = FileConfigStore::new(path);
        assert!(matches!(store.load(), Err(ConfigError::Parse(_))));
    }
}
