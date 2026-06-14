#ifndef KtMonotonicCountdown_H
#define KtMonotonicCountdown_H

#include <QtGlobal>

class KtMonotonicCountdown {
public:
    KtMonotonicCountdown();
    ~KtMonotonicCountdown() = default;

    int get_counter() const {
        return counter_;
    }

    bool get_running() const {
        return running_;
    }

    int get_state() const {
        return state_;
    }

    bool is_expired() const {
        return running_ && counter_ <= 0;
    }

    void pause();
    void reset(int state, int counterSec);
    void start(int state, int counterSec);
    void sync_from_monotonic_clock();

private:
    void update_remaining_from_monotonic_clock();

private:
    int    state_;
    int    counter_;
    int    phaseDurationSec_;
    qint64 phaseStartMs_;
    bool   running_;
};

#endif // KtMonotonicCountdown_H
