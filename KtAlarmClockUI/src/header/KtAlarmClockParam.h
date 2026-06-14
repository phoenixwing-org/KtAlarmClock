#ifndef KtAlarmClockParam_H
#define KtAlarmClockParam_H

#include "KtAlarmClockUI.h"

#include <memory>

class ExportedByKtAlarmClockUI KtAlarmClockParam {
public:
    KtAlarmClockParam();
    virtual ~KtAlarmClockParam();

    KtAlarmClockParam(const KtAlarmClockParam&) = default;
    KtAlarmClockParam& operator=(const KtAlarmClockParam&) = default;

    void setDefault();
    void load();
    void normalize();
    void save();

public:
    int WorkTime;
    int WorkBreak;
    int TimeForce;
    int TimeTotal;
    int WorkStep;
    int TimeCounter;
};

using KtAlarmClockParamShared = std::shared_ptr<KtAlarmClockParam>;

#endif // KtAlarmClockParam_H
