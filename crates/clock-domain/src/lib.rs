//! Clock 的纯业务 Model。
//!
//! 本 crate 不依赖 UI、操作系统 API 或异步运行时。所有时间都由调用者以秒级
//! [`TimePoint`] 注入，因此单元测试不需要等待真实时间。

mod model;
mod settings;

pub use model::{ClockModel, Phase, Snapshot, TimePoint};
pub use settings::{
    Settings, SettingsError, format_duration, parse_duration_in_range, parse_duration_text,
};
