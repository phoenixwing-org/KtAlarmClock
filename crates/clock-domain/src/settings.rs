use std::error::Error;
use std::fmt::{Display, Formatter};

/// 当前配置文件结构版本。
pub const SETTINGS_SCHEMA_VERSION: u32 = 1;

/// 产品允许的时长范围，与旧 Qt 设置界面保持一致。
pub const MIN_WORK_SECONDS: u32 = 60;
pub const MAX_WORK_SECONDS: u32 = 5_400;
pub const MIN_BREAK_SECONDS: u32 = 10;
pub const MAX_BREAK_SECONDS: u32 = 3_600;
pub const MAX_FORCE_SECONDS: u32 = 3_600;

/// 解析旧 Qt `KtDurationEdit` 接受的时长文本。
///
/// 支持 `分:秒`、带 `s`/`S` 后缀的秒数和表示分钟的纯数字；其余输入无效。
pub fn parse_duration_text(text: &str) -> Option<u32> {
    let body = text.trim();
    if body.is_empty() {
        return None;
    }

    if let Some(seconds) = body.strip_suffix('s').or_else(|| body.strip_suffix('S')) {
        return seconds.trim().parse().ok();
    }

    if let Some((minutes, seconds)) = body.split_once(':') {
        let minutes: u32 = minutes.trim().parse().ok()?;
        let seconds: u32 = seconds.trim().parse().ok()?;
        if seconds > 59 {
            return None;
        }
        return minutes.checked_mul(60)?.checked_add(seconds);
    }

    body.parse::<u32>().ok()?.checked_mul(60)
}

/// 按旧 Qt 控件的行为解析输入，并将合法结果限制到控件范围内。
pub fn parse_duration_in_range(text: &str, minimum: u32, maximum: u32) -> Option<u32> {
    let (minimum, maximum) = if minimum <= maximum {
        (minimum, maximum)
    } else {
        (maximum, minimum)
    };
    parse_duration_text(text).map(|seconds| seconds.clamp(minimum, maximum))
}

/// 使用旧 Qt 控件的规范格式显示秒数。
pub fn format_duration(seconds: u32) -> String {
    format!("{}:{:02}", seconds / 60, seconds % 60)
}

/// 与 UI/存储无关的用户设置。
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Settings {
    pub schema_version: u32,
    pub work_seconds: u32,
    pub break_seconds: u32,
    pub force_seconds: u32,
    pub auto_start: bool,
}

impl Settings {
    pub fn validate(self) -> Result<Self, SettingsError> {
        if self.schema_version != SETTINGS_SCHEMA_VERSION {
            return Err(SettingsError::UnsupportedSchema(self.schema_version));
        }
        validate_range(
            "work_seconds",
            self.work_seconds,
            MIN_WORK_SECONDS,
            MAX_WORK_SECONDS,
        )?;
        validate_range(
            "break_seconds",
            self.break_seconds,
            MIN_BREAK_SECONDS,
            MAX_BREAK_SECONDS,
        )?;
        validate_range("force_seconds", self.force_seconds, 0, MAX_FORCE_SECONDS)?;
        if self.force_seconds > self.break_seconds {
            return Err(SettingsError::ForceLongerThanBreak {
                force_seconds: self.force_seconds,
                break_seconds: self.break_seconds,
            });
        }
        Ok(self)
    }
}

impl Default for Settings {
    fn default() -> Self {
        Self {
            schema_version: SETTINGS_SCHEMA_VERSION,
            work_seconds: 45 * 60,
            break_seconds: 10 * 60,
            force_seconds: 5 * 60,
            // 旧版本总是创建自启动项；新版本保留默认行为，但允许用户关闭。
            auto_start: true,
        }
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum SettingsError {
    UnsupportedSchema(u32),
    OutOfRange {
        field: &'static str,
        value: u32,
        min: u32,
        max: u32,
    },
    ForceLongerThanBreak {
        force_seconds: u32,
        break_seconds: u32,
    },
}

impl Display for SettingsError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::UnsupportedSchema(version) => {
                write!(f, "不支持的配置版本: {version}")
            }
            Self::OutOfRange {
                field,
                value,
                min,
                max,
            } => write!(f, "{field}={value} 超出范围 {min}..={max}"),
            Self::ForceLongerThanBreak {
                force_seconds,
                break_seconds,
            } => write!(
                f,
                "force_seconds={force_seconds} 不能大于 break_seconds={break_seconds}"
            ),
        }
    }
}

impl Error for SettingsError {}

fn validate_range(
    field: &'static str,
    value: u32,
    min: u32,
    max: u32,
) -> Result<(), SettingsError> {
    if (min..=max).contains(&value) {
        Ok(())
    } else {
        Err(SettingsError::OutOfRange {
            field,
            value,
            min,
            max,
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn defaults_are_valid() {
        assert_eq!(Settings::default().validate(), Ok(Settings::default()));
    }

    #[test]
    fn force_may_equal_the_whole_break() {
        let settings = Settings {
            force_seconds: 600,
            ..Settings::default()
        };
        assert!(settings.validate().is_ok());
    }

    #[test]
    fn force_cannot_exceed_break() {
        let settings = Settings {
            force_seconds: 601,
            ..Settings::default()
        };
        assert!(matches!(
            settings.validate(),
            Err(SettingsError::ForceLongerThanBreak { .. })
        ));
    }

    #[test]
    fn duration_text_rules_match_the_qt_editor() {
        assert_eq!(parse_duration_text("45:00"), Some(2_700));
        assert_eq!(parse_duration_text(" 3:07 "), Some(187));
        assert_eq!(parse_duration_text("3: 07"), Some(187));
        assert_eq!(parse_duration_text("10s"), Some(10));
        assert_eq!(parse_duration_text("10S"), Some(10));
        assert_eq!(parse_duration_text("45"), Some(2_700));
        assert_eq!(parse_duration_text("0"), Some(0));

        for invalid in ["", " ", "1:60", "1:2:3", "-1", "-1s", "abc"] {
            assert_eq!(parse_duration_text(invalid), None, "{invalid}");
        }
    }

    #[test]
    fn duration_text_is_clamped_without_rounding() {
        assert_eq!(parse_duration_in_range("1s", 10, 3_600), Some(10));
        assert_eq!(parse_duration_in_range("90", 60, 5_400), Some(5_400));
        assert_eq!(parse_duration_in_range("1:23", 60, 5_400), Some(83));
        assert_eq!(format_duration(83), "1:23");
        assert_eq!(format_duration(0), "0:00");
    }
}
