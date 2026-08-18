use clock_domain::{ClockModel, Settings, SettingsError, Snapshot, TimePoint};

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum UserAction {
    PlayPause,
    StartBreak,
    NextWork,
    AdjustWork(i32),
    ShowSettings,
    Quit,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Event {
    User(UserAction),
    Tick,
    Suspended,
    Resumed,
    Unlock,
    DebugExit,
    SecondInstance,
    ApplySettings(Settings),
}

/// Controller 请求 View/平台执行的副作用。
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Effect {
    ShowMain,
    HideMain,
    ShowSettings,
    ShowBreakOverlays,
    DismissBreakOverlays,
    ActivateMain,
    PersistSettings(Settings),
    SetAutoStart(bool),
    Exit,
}

#[derive(Debug, Clone)]
pub struct Controller {
    model: ClockModel,
}

impl Controller {
    pub fn new(settings: Settings) -> Result<Self, SettingsError> {
        Ok(Self {
            model: ClockModel::new(settings.validate()?),
        })
    }

    pub fn settings(&self) -> Settings {
        self.model.settings()
    }

    pub fn snapshot(&self, now: TimePoint) -> Snapshot {
        self.model.snapshot(now)
    }

    pub fn dispatch(&mut self, event: Event, now: TimePoint) -> Result<Vec<Effect>, SettingsError> {
        let effects = match event {
            Event::User(action) => self.handle_user_action(action, now),
            Event::Tick => {
                if self.model.tick(now) {
                    vec![Effect::ShowBreakOverlays, Effect::HideMain]
                } else {
                    Vec::new()
                }
            }
            Event::Suspended => {
                self.model.suspend(now);
                Vec::new()
            }
            Event::Resumed => {
                self.model.resume(now);
                Vec::new()
            }
            Event::Unlock => {
                if self.model.unlock_break(now) {
                    vec![Effect::DismissBreakOverlays, Effect::ShowMain]
                } else {
                    Vec::new()
                }
            }
            Event::DebugExit => {
                if self.model.debug_exit_break(now) {
                    vec![Effect::DismissBreakOverlays, Effect::ShowMain]
                } else {
                    Vec::new()
                }
            }
            Event::SecondInstance => {
                if self.model.phase() == clock_domain::Phase::Break {
                    Vec::new()
                } else {
                    vec![Effect::ShowMain, Effect::ActivateMain]
                }
            }
            Event::ApplySettings(settings) => {
                let settings = settings.validate()?;
                let auto_start_changed = settings.auto_start != self.model.settings().auto_start;
                self.model.replace_settings(settings);
                let mut effects = vec![Effect::PersistSettings(settings)];
                if auto_start_changed {
                    effects.push(Effect::SetAutoStart(settings.auto_start));
                }
                effects
            }
        };
        Ok(effects)
    }

    fn handle_user_action(&mut self, action: UserAction, now: TimePoint) -> Vec<Effect> {
        if self.model.phase() == clock_domain::Phase::Break {
            return match action {
                UserAction::Quit => vec![Effect::DismissBreakOverlays, Effect::Exit],
                _ => Vec::new(),
            };
        }

        match action {
            UserAction::PlayPause => {
                if self.model.is_running() {
                    self.model.pause_work(now);
                } else {
                    self.model.start_or_resume_work(now);
                }
                vec![Effect::ShowMain]
            }
            UserAction::StartBreak => {
                if self.model.start_break(now) {
                    vec![Effect::ShowBreakOverlays, Effect::HideMain]
                } else {
                    Vec::new()
                }
            }
            UserAction::NextWork => {
                self.model.reset_and_start_work(now);
                vec![Effect::DismissBreakOverlays, Effect::ShowMain]
            }
            UserAction::AdjustWork(delta) => {
                self.model.adjust_work(delta, now);
                vec![Effect::DismissBreakOverlays, Effect::ShowMain]
            }
            UserAction::ShowSettings => vec![Effect::ShowSettings],
            UserAction::Quit => vec![Effect::DismissBreakOverlays, Effect::Exit],
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use clock_domain::Phase;

    fn time(seconds: u64) -> TimePoint {
        TimePoint::new(50_000 + seconds as i64, 2_000 + seconds)
    }

    fn controller() -> Controller {
        Controller::new(Settings::default()).unwrap()
    }

    #[test]
    fn play_pause_and_resume_keep_remaining_time() {
        let mut controller = controller();
        controller
            .dispatch(Event::User(UserAction::PlayPause), time(0))
            .unwrap();
        controller
            .dispatch(Event::User(UserAction::PlayPause), time(30))
            .unwrap();
        assert_eq!(controller.snapshot(time(300)).remaining_seconds, 2_670);
        controller
            .dispatch(Event::User(UserAction::PlayPause), time(300))
            .unwrap();
        assert_eq!(controller.snapshot(time(360)).remaining_seconds, 2_610);
    }

    #[test]
    fn work_timeout_enters_break_once() {
        let settings = Settings {
            work_seconds: 60,
            ..Settings::default()
        };
        let mut controller = Controller::new(settings).unwrap();
        controller
            .dispatch(Event::User(UserAction::PlayPause), time(0))
            .unwrap();
        assert_eq!(
            controller.dispatch(Event::Tick, time(60)).unwrap(),
            vec![Effect::ShowBreakOverlays, Effect::HideMain]
        );
        assert!(
            controller
                .dispatch(Event::Tick, time(61))
                .unwrap()
                .is_empty()
        );
        assert_eq!(controller.snapshot(time(61)).phase, Phase::Break);
    }

    #[test]
    fn duplicate_unlock_is_idempotent() {
        let settings = Settings {
            force_seconds: 0,
            ..Settings::default()
        };
        let mut controller = Controller::new(settings).unwrap();
        controller
            .dispatch(Event::User(UserAction::StartBreak), time(0))
            .unwrap();
        assert_eq!(
            controller.dispatch(Event::Unlock, time(1)).unwrap(),
            vec![Effect::DismissBreakOverlays, Effect::ShowMain]
        );
        assert!(
            controller
                .dispatch(Event::Unlock, time(1))
                .unwrap()
                .is_empty()
        );
    }

    #[test]
    fn duplicate_debug_exit_is_idempotent() {
        let mut controller = controller();
        controller
            .dispatch(Event::User(UserAction::StartBreak), time(0))
            .unwrap();
        assert_eq!(
            controller.dispatch(Event::DebugExit, time(1)).unwrap(),
            vec![Effect::DismissBreakOverlays, Effect::ShowMain]
        );
        assert!(
            controller
                .dispatch(Event::DebugExit, time(1))
                .unwrap()
                .is_empty()
        );
    }

    #[test]
    fn settings_change_requests_persistence_and_autostart_update() {
        let mut controller = controller();
        let settings = Settings {
            auto_start: false,
            ..Settings::default()
        };
        assert_eq!(
            controller
                .dispatch(Event::ApplySettings(settings), time(0))
                .unwrap(),
            vec![
                Effect::PersistSettings(settings),
                Effect::SetAutoStart(false)
            ]
        );
    }

    #[test]
    fn next_work_uses_the_just_applied_duration() {
        let mut controller = controller();
        let settings = Settings {
            work_seconds: 800,
            ..Settings::default()
        };
        controller
            .dispatch(Event::ApplySettings(settings), time(0))
            .unwrap();
        controller
            .dispatch(Event::User(UserAction::NextWork), time(0))
            .unwrap();
        assert_eq!(controller.snapshot(time(0)).remaining_seconds, 800);
        assert!(controller.snapshot(time(0)).running);
    }

    #[test]
    fn adjust_work_is_seconds_based() {
        let mut controller = controller();
        controller
            .dispatch(Event::User(UserAction::AdjustWork(-60)), time(0))
            .unwrap();
        assert_eq!(controller.snapshot(time(0)).remaining_seconds, 2_640);
        controller
            .dispatch(Event::User(UserAction::AdjustWork(60)), time(1))
            .unwrap();
        assert_eq!(controller.snapshot(time(1)).remaining_seconds, 2_699);
    }
}
