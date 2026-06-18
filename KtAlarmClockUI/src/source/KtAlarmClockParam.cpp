#include "KtAlarmClockParam.h"

#include "KtAlarmClock.h"
#include "KtAlarmClockSettings.h"

#include <QDebug>

//----------------------------------------
KtAlarmClockParam::KtAlarmClockParam()
    : WorkTime(KtAlarmClockSettings::DefaultWorkTime)
    , WorkBreak(KtAlarmClockSettings::DefaultWorkBreak)
    , TimeForce(KtAlarmClockSettings::DefaultTimeForce)
    , TimeTotal(KtAlarmClockSettings::DefaultWorkTime + KtAlarmClockSettings::DefaultWorkBreak)
    , WorkStep(KtAlarmClock::None)
    , TimeCounter(0) {
}
//----------------------------------------
KtAlarmClockParam::~KtAlarmClockParam() = default;
//----------------------------------------
void KtAlarmClockParam::load() {
    qDebug() << "KtAlarmClockParam::load() ";
    KtAlarmClockSettings::read(*this);
}
//----------------------------------------
void KtAlarmClockParam::normalize() {
    KtAlarmClockSettings::normalize(*this);
}
//----------------------------------------
void KtAlarmClockParam::save() {
    qDebug() << "KtAlarmClockParam::save() ";
    KtAlarmClockSettings::write(*this);
}
//----------------------------------------
void KtAlarmClockParam::setDefault() {
    KtAlarmClockSettings::apply_defaults(*this);
}
