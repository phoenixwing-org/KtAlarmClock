/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @file        KtAlarmClockSettings.h
 */
#ifndef KtAlarmClockSettings_H
#define KtAlarmClockSettings_H

#include "KtAlarmClockUI.h"

#include <QString>

class KtAlarmClockParam;

struct ExportedByKtAlarmClockUI KtDurationBounds {
    int minimum;
    int maximum;
    int fallback;
};

class ExportedByKtAlarmClockUI KtAlarmClockSettings {
public:
    static constexpr int DefaultWorkTime  = 2700;
    static constexpr int DefaultWorkBreak = 600;
    static constexpr int DefaultTimeForce = 600;

    static constexpr KtDurationBounds work_time_bounds() {
        return {60, 5400, DefaultWorkTime};
    }

    static constexpr KtDurationBounds work_break_bounds() {
        return {10, 3600, DefaultWorkBreak};
    }

    static constexpr KtDurationBounds time_force_bounds() {
        return {0, 3600, DefaultTimeForce};
    }

    static void apply_defaults(KtAlarmClockParam& param);
    static QString file_path();
    static QString normalize_locale(const QString& localeName);
    static QString read_locale();
    static void normalize(KtAlarmClockParam& param);
    static void read(KtAlarmClockParam& param);
    static void write_locale(const QString& localeName);
    static void write(KtAlarmClockParam& param);
};

#endif // KtAlarmClockSettings_H
