#include "KtMonotonicCountdown.h"

#include "KtQpcClock.h"

#include <algorithm>

KtMonotonicCountdown::KtMonotonicCountdown()
    : state_(0)
    , counter_(0)
    , phaseDurationSec_(0)
    , phaseStartMs_(0)
    , running_(false) {
}

void KtMonotonicCountdown::pause() {
    if (running_) update_remaining_from_monotonic_clock();

    running_      = false;
    phaseStartMs_ = 0;
    if (counter_ > 0) phaseDurationSec_ = counter_;
}

void KtMonotonicCountdown::reset(int state, int counterSec) {
    state_            = state;
    counter_          = counterSec;
    phaseDurationSec_ = counterSec;
    phaseStartMs_     = 0;
    running_          = false;
}

void KtMonotonicCountdown::start(int state, int counterSec) {
    state_   = state;
    counter_ = counterSec;

    if (counter_ <= 0) {
        running_ = false;
        return;
    }

    phaseDurationSec_ = counter_;
    phaseStartMs_     = KtQpcClock::now_ms();
    update_remaining_from_monotonic_clock();
    running_ = true;
}

void KtMonotonicCountdown::sync_from_monotonic_clock() {
    if (!running_) return;
    update_remaining_from_monotonic_clock();
}

void KtMonotonicCountdown::update_remaining_from_monotonic_clock() {
    if (state_ == 0 || phaseDurationSec_ <= 0 || phaseStartMs_ <= 0) return;

    const qint64 elapsedMs  = KtQpcClock::now_ms() - phaseStartMs_;
    const int    elapsedSec = static_cast<int>(elapsedMs / 1000);
    counter_ = std::max(0, phaseDurationSec_ - elapsedSec);
}
