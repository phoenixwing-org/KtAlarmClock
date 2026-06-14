#include "KtAlarmClockViewModel.h"

#include "KtAlarmClock.h"
#include "KtAlarmClockParam.h"

KtAlarmClockViewModel::KtAlarmClockViewModel(QObject* parent)
    : QObject(parent)
    , m_phase(KtAlarmClock::None)
    , m_running(false)
    , m_loop(false)
    , m_forbidden(false)
    , m_lockVisible(false)
    , m_remainingSeconds(0)
    , m_remainingText(QStringLiteral("0:00"))
    , m_workSeconds(0)
    , m_breakSeconds(0)
    , m_forceSeconds(0)
    , m_settingsVisible(false)
    , m_locale(QStringLiteral("zh_CN")) {
}

QString KtAlarmClockViewModel::formatDuration(int seconds) const {
    if (seconds <= 0)
        return QStringLiteral("0:00");

    const int minutePart = seconds / 60;
    const int secondPart = seconds % 60;
    return QStringLiteral("%1:%2").arg(minutePart).arg(secondPart, 2, 10, QChar('0'));
}

void KtAlarmClockViewModel::set_phase_state(int phaseValue) {
    phase(phaseValue);
    forbidden(phaseValue == KtAlarmClock::WorkBreak);
}

void KtAlarmClockViewModel::set_remaining_seconds(int seconds) {
    if (seconds < 0)
        seconds = 0;
    remainingSeconds(seconds);
    remainingText(formatDuration(seconds));
}

void KtAlarmClockViewModel::set_running_state(bool runningValue) {
    running(runningValue);
}

void KtAlarmClockViewModel::sync_from_param(const KtAlarmClockParam& param) {
    workSeconds(param.WorkTime);
    breakSeconds(param.WorkBreak);
    forceSeconds(param.TimeForce);
}
