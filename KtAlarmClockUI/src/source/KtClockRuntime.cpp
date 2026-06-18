#include "KtClockRuntime.h"

#include "KtAlarmClock.h"

#include <QTimer>

namespace {
constexpr int kInitialCounter = -100;
}

//----------------------------------------
KtClockRuntime::KtClockRuntime(QObject* parent)
    : QObject(parent)
    , clock_()
    , timer_(new QTimer(this)) {
    clock_.reset(KtAlarmClock::None, kInitialCounter);
    timer_->setInterval(1000);
    connect(timer_, &QTimer::timeout, this, &KtClockRuntime::on_tick);
}
//----------------------------------------
KtClockRuntime::~KtClockRuntime() = default;
//----------------------------------------
int KtClockRuntime::counter() const {
    return clock_.get_counter();
}
//----------------------------------------
void KtClockRuntime::emit_updated() {
    emit updated(clock_.get_state(), clock_.get_counter(), clock_.get_running());
}
//----------------------------------------
void KtClockRuntime::on_tick() {
    sync_from_monotonic_clock();
}
//----------------------------------------
void KtClockRuntime::pause() {
    clock_.pause();
    timer_->stop();
    emit_updated();
}
//----------------------------------------
int KtClockRuntime::phase() const {
    return clock_.get_state();
}
//----------------------------------------
bool KtClockRuntime::running() const {
    return clock_.get_running();
}
//----------------------------------------
void KtClockRuntime::start(int phase, int counterSec) {
    clock_.start(phase, counterSec);
    if (clock_.get_running())
        timer_->start();
    else
        timer_->stop();
    emit_updated();
}
//----------------------------------------
void KtClockRuntime::sync_from_monotonic_clock() {
    if (!clock_.get_running())
        return;

    clock_.sync_from_monotonic_clock();
    emit_updated();

    if (!clock_.is_expired())
        return;

    const int expiredPhase = clock_.get_state();
    clock_.pause();
    timer_->stop();
    emit_updated();
    emit timeout(expiredPhase);
}
