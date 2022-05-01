/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockCore.h
 * @version		V1.0
 * @brief		Core class
 */

#ifndef KTQAlarmClockCore_H
#define KTQAlarmClockCore_H

// Qt
#include <QObject>

// KTQ
#include "KTQAlarmClockUI.h"

// class pre-declare
class KTQAlarmClockParam;

/**
 * @brief Class KTQAlarmClockCore
 */
class ExportedByKTQAlarmClockUI KTQAlarmClockCore {
    Q_DISABLE_COPY(KTQAlarmClockCore);

public:
    explicit KTQAlarmClockCore();
    ~KTQAlarmClockCore();

public:
    /**
     * @brief pretreat
     * @return int, 0: succeed, plus： failed
     */
    int pretreat();

    /**
     * @brief calculate
     * @return int, 0: succeed, plus： failed
     */
    int calculate();

public:
    KTQAlarmClockParam *k_pClockParam;

private:
    int m_Pretreat;
};

#endif // KTQAlarmClockCore_H
