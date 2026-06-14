/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @file        KtQpcClock.h
 * @brief       Monotonic millisecond clock for countdown intervals.
 */
#ifndef KtQpcClock_H
#define KtQpcClock_H

#include <QtGlobal>

/**
 * @brief Monotonic clock used for elapsed-time countdowns.
 *
 * On Windows this uses QueryPerformanceCounter. Values are suitable for interval
 * calculations only and are not synchronized to UTC or local system time.
 */
class KtQpcClock {
public:
    /** @brief Current monotonic timestamp in milliseconds. */
    static qint64 now_ms();
};

#endif // KtQpcClock_H
