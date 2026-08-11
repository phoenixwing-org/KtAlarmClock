use crate::Settings;

/// UI 与平台层注入的秒级时间。
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct TimePoint {
    /// Unix 时间秒。休息期使用它，使系统休眠仍计入休息。
    pub wall_seconds: i64,
    /// 单调时钟秒。工作期使用它，避免系统时间校准改变工作剩余时间。
    pub monotonic_seconds: u64,
}

impl TimePoint {
    pub const fn new(wall_seconds: i64, monotonic_seconds: u64) -> Self {
        Self {
            wall_seconds,
            monotonic_seconds,
        }
    }
}

#[derive(Debug, Clone, Copy, Default, PartialEq, Eq)]
pub enum Phase {
    #[default]
    Idle,
    Work,
    Break,
}

/// View 所需的只读快照。
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Snapshot {
    pub phase: Phase,
    pub running: bool,
    pub remaining_seconds: u32,
    pub forced: bool,
    pub can_unlock: bool,
}

/// 纯业务状态；不直接创建窗口或执行平台副作用。
#[derive(Debug, Clone)]
pub struct ClockModel {
    settings: Settings,
    phase: Phase,
    running: bool,
    work_remaining: u32,
    work_started_at: Option<u64>,
    work_started_with: u32,
    resume_work_after_suspend: bool,
    break_ends_at: Option<i64>,
    force_ends_at: Option<i64>,
    last_break_wall: Option<i64>,
}

impl ClockModel {
    pub fn new(settings: Settings) -> Self {
        debug_assert!(settings.validate().is_ok());
        Self {
            settings,
            phase: Phase::Idle,
            running: false,
            work_remaining: settings.work_seconds,
            work_started_at: None,
            work_started_with: settings.work_seconds,
            resume_work_after_suspend: false,
            break_ends_at: None,
            force_ends_at: None,
            last_break_wall: None,
        }
    }

    pub const fn settings(&self) -> Settings {
        self.settings
    }

    pub fn replace_settings(&mut self, settings: Settings) {
        debug_assert!(settings.validate().is_ok());
        self.settings = settings;
        if self.phase == Phase::Idle && !self.running {
            self.work_remaining = self.work_remaining.min(settings.work_seconds);
        }
    }

    pub const fn phase(&self) -> Phase {
        self.phase
    }

    pub const fn is_running(&self) -> bool {
        self.running
    }

    pub fn snapshot(&self, now: TimePoint) -> Snapshot {
        match self.phase {
            Phase::Break => {
                let wall = self.effective_break_wall(now.wall_seconds);
                let remaining = seconds_until(self.break_ends_at, wall);
                let forced = seconds_until(self.force_ends_at, wall) > 0;
                Snapshot {
                    phase: Phase::Break,
                    running: true,
                    remaining_seconds: remaining,
                    forced,
                    can_unlock: !forced,
                }
            }
            Phase::Work | Phase::Idle => Snapshot {
                phase: self.phase,
                running: self.running,
                remaining_seconds: self.current_work_remaining(now.monotonic_seconds),
                forced: false,
                can_unlock: false,
            },
        }
    }

    /// 开始或继续工作。返回是否发生了状态变化。
    pub fn start_or_resume_work(&mut self, now: TimePoint) -> bool {
        if self.phase == Phase::Break || self.running {
            return false;
        }
        if self.work_remaining == 0 {
            self.work_remaining = self.settings.work_seconds;
        }
        self.phase = Phase::Work;
        self.running = true;
        self.work_started_at = Some(now.monotonic_seconds);
        self.work_started_with = self.work_remaining;
        self.resume_work_after_suspend = false;
        true
    }

    pub fn pause_work(&mut self, now: TimePoint) -> bool {
        if self.phase != Phase::Work || !self.running {
            return false;
        }
        self.sync_work(now.monotonic_seconds);
        self.running = false;
        self.work_started_at = None;
        self.phase = Phase::Idle;
        self.resume_work_after_suspend = false;
        true
    }

    pub fn reset_and_start_work(&mut self, now: TimePoint) {
        self.leave_break();
        self.phase = Phase::Work;
        self.running = true;
        self.work_remaining = self.settings.work_seconds;
        self.work_started_with = self.work_remaining;
        self.work_started_at = Some(now.monotonic_seconds);
        self.resume_work_after_suspend = false;
    }

    pub fn adjust_work(&mut self, delta_seconds: i32, now: TimePoint) -> bool {
        if self.phase == Phase::Break {
            return false;
        }
        let current = self.current_work_remaining(now.monotonic_seconds);
        let adjusted = if delta_seconds >= 0 {
            current.saturating_add(delta_seconds.unsigned_abs())
        } else {
            current.saturating_sub(delta_seconds.unsigned_abs())
        };
        self.work_remaining = adjusted.min(24 * 60 * 60);
        self.phase = Phase::Work;
        self.running = self.work_remaining > 0;
        self.work_started_with = self.work_remaining;
        self.work_started_at = self.running.then_some(now.monotonic_seconds);
        self.resume_work_after_suspend = false;
        true
    }

    pub fn start_break(&mut self, now: TimePoint) -> bool {
        if self.phase == Phase::Break {
            return false;
        }
        self.sync_work(now.monotonic_seconds);
        self.phase = Phase::Break;
        self.running = true;
        self.work_started_at = None;
        self.resume_work_after_suspend = false;
        self.last_break_wall = Some(now.wall_seconds);
        self.break_ends_at = Some(
            now.wall_seconds
                .saturating_add(self.settings.break_seconds.into()),
        );
        self.force_ends_at = Some(
            now.wall_seconds
                .saturating_add(self.settings.force_seconds.into()),
        );
        true
    }

    /// 推进秒级状态。工作到期时返回 `true`，调用者应创建休息遮罩。
    pub fn tick(&mut self, now: TimePoint) -> bool {
        match self.phase {
            Phase::Work if self.running => {
                self.sync_work(now.monotonic_seconds);
                if self.work_remaining == 0 {
                    self.start_break(now);
                    true
                } else {
                    false
                }
            }
            Phase::Break => {
                self.observe_break_wall(now.wall_seconds);
                false
            }
            Phase::Idle | Phase::Work => false,
        }
    }

    /// 工作中休眠冻结；休息中只记录当前墙钟，deadline 不移动。
    pub fn suspend(&mut self, now: TimePoint) {
        if self.phase == Phase::Work && self.running {
            self.sync_work(now.monotonic_seconds);
            self.running = false;
            self.work_started_at = None;
            self.resume_work_after_suspend = self.work_remaining > 0;
        } else if self.phase == Phase::Break {
            self.observe_break_wall(now.wall_seconds);
        }
    }

    /// 恢复后工作从冻结点继续；休息按绝对墙钟立即补算。
    pub fn resume(&mut self, now: TimePoint) {
        if self.phase == Phase::Work && self.resume_work_after_suspend {
            self.running = true;
            self.work_started_at = Some(now.monotonic_seconds);
            self.work_started_with = self.work_remaining;
            self.resume_work_after_suspend = false;
        } else if self.phase == Phase::Break {
            self.observe_break_wall(now.wall_seconds);
        }
    }

    /// 达到强制期后允许解锁。重复调用在离开 Break 后返回 false。
    pub fn unlock_break(&mut self, now: TimePoint) -> bool {
        if self.phase != Phase::Break || !self.snapshot(now).can_unlock {
            return false;
        }
        self.reset_and_start_work(now);
        true
    }

    /// 调试退出不受强制期限制，但仍保持幂等。
    pub fn debug_exit_break(&mut self, now: TimePoint) -> bool {
        if self.phase != Phase::Break {
            return false;
        }
        self.reset_and_start_work(now);
        true
    }

    fn current_work_remaining(&self, monotonic_seconds: u64) -> u32 {
        if !self.running || self.phase != Phase::Work {
            return self.work_remaining;
        }
        let elapsed = self
            .work_started_at
            .map_or(0, |start| monotonic_seconds.saturating_sub(start));
        self.work_started_with
            .saturating_sub(elapsed.min(u32::MAX.into()) as u32)
    }

    fn sync_work(&mut self, monotonic_seconds: u64) {
        self.work_remaining = self.current_work_remaining(monotonic_seconds);
        self.work_started_with = self.work_remaining;
        if self.running {
            self.work_started_at = Some(monotonic_seconds);
        }
    }

    fn effective_break_wall(&self, wall_seconds: i64) -> i64 {
        self.last_break_wall
            .map_or(wall_seconds, |last| last.max(wall_seconds))
    }

    fn observe_break_wall(&mut self, wall_seconds: i64) {
        let effective = self.effective_break_wall(wall_seconds);
        self.last_break_wall = Some(effective);
    }

    fn leave_break(&mut self) {
        self.break_ends_at = None;
        self.force_ends_at = None;
        self.last_break_wall = None;
    }
}

fn seconds_until(deadline: Option<i64>, wall_seconds: i64) -> u32 {
    deadline
        .map(|end| end.saturating_sub(wall_seconds).clamp(0, u32::MAX.into()) as u32)
        .unwrap_or(0)
}

#[cfg(test)]
mod tests {
    use super::*;

    fn time(seconds: u64) -> TimePoint {
        TimePoint::new(10_000 + seconds as i64, 1_000 + seconds)
    }

    #[test]
    fn work_uses_monotonic_seconds() {
        let mut model = ClockModel::new(Settings::default());
        assert!(model.start_or_resume_work(time(0)));
        assert_eq!(model.snapshot(time(59)).remaining_seconds, 2_641);
    }

    #[test]
    fn work_freezes_during_suspend() {
        let mut model = ClockModel::new(Settings::default());
        model.start_or_resume_work(time(0));
        model.suspend(time(120));
        assert_eq!(model.snapshot(time(7_200)).remaining_seconds, 2_580);
        model.resume(time(7_200));
        assert_eq!(model.snapshot(time(7_260)).remaining_seconds, 2_520);
    }

    #[test]
    fn manually_paused_work_does_not_resume_after_suspend() {
        let mut model = ClockModel::new(Settings::default());
        model.start_or_resume_work(time(0));
        model.pause_work(time(10));
        model.suspend(time(20));
        model.resume(time(200));
        assert!(!model.is_running());
        assert_eq!(model.snapshot(time(200)).remaining_seconds, 2_690);
    }

    #[test]
    fn break_and_force_continue_during_suspend() {
        let mut model = ClockModel::new(Settings::default());
        model.start_break(time(0));
        model.suspend(time(60));
        model.resume(time(360));
        let snapshot = model.snapshot(time(360));
        assert_eq!(snapshot.remaining_seconds, 240);
        assert!(!snapshot.forced);
        assert!(snapshot.can_unlock);
    }

    #[test]
    fn zero_force_allows_immediate_unlock() {
        let settings = Settings {
            force_seconds: 0,
            ..Settings::default()
        };
        let mut model = ClockModel::new(settings);
        model.start_break(time(0));
        let snapshot = model.snapshot(time(0));
        assert!(!snapshot.forced);
        assert!(snapshot.can_unlock);
    }

    #[test]
    fn force_equal_to_break_lasts_until_break_end() {
        let settings = Settings {
            force_seconds: Settings::default().break_seconds,
            ..Settings::default()
        };
        let mut model = ClockModel::new(settings);
        model.start_break(time(0));
        assert!(model.snapshot(time(599)).forced);
        assert!(!model.snapshot(time(599)).can_unlock);
        assert!(!model.snapshot(time(600)).forced);
        assert!(model.snapshot(time(600)).can_unlock);
    }

    #[test]
    fn wall_clock_rollback_never_increases_break_remaining() {
        let mut model = ClockModel::new(Settings::default());
        model.start_break(time(0));
        model.tick(time(100));
        let before = model.snapshot(time(100)).remaining_seconds;
        let rolled_back = TimePoint::new(time(50).wall_seconds, time(101).monotonic_seconds);
        assert_eq!(model.snapshot(rolled_back).remaining_seconds, before);
    }
}
