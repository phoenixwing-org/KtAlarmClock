/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @file        KtQpcClock.cpp
 */
#include "KtQpcClock.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <QDateTime>
#endif

namespace {

#ifdef _WIN32
qint64 qpc_frequency() {
    static const qint64 frequency = []() {
        LARGE_INTEGER value {};
        if (!QueryPerformanceFrequency(&value))
            return qint64(0);
        return static_cast<qint64>(value.QuadPart);
    }();
    return frequency;
}

qint64 ticks_to_ms(qint64 ticks, qint64 frequency) {
    if (frequency <= 0)
        return 0;
    return (ticks / frequency) * 1000 + ((ticks % frequency) * 1000) / frequency;
}

qint64 tick_count_ms() {
    return static_cast<qint64>(GetTickCount64());
}
#endif

} // namespace

//------------------------------------------------------
qint64 KtQpcClock::now_ms() {
#ifdef _WIN32
    LARGE_INTEGER value {};
    if (!QueryPerformanceCounter(&value))
        return tick_count_ms();

    const qint64 frequency = qpc_frequency();
    if (frequency <= 0)
        return tick_count_ms();

    return ticks_to_ms(static_cast<qint64>(value.QuadPart), frequency);
#else
    return QDateTime::currentMSecsSinceEpoch();
#endif
}
