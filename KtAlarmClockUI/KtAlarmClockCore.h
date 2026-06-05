/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockCore.h
 * @version		V1.0
 * @brief		Core class
 */

#ifndef KtAlarmClockCore_H
#define KtAlarmClockCore_H

// Qt
#include <QObject>

// Kt
#include "KtAlarmClockParam.h"
#include "KtAlarmClockUI.h"

/**
 * @brief Class KtAlarmClockCore
 */
class ExportedByKtAlarmClockUI KtAlarmClockCore {
    Q_DISABLE_COPY(KtAlarmClockCore);

public:
    explicit KtAlarmClockCore();
    ~KtAlarmClockCore();

public:
    /**
     * @brief pretreat
     * @return int, 0: succeed, plus: failed
     */
    int pretreat();

    /**
     * @brief calculate
     * @return int, 0: succeed, plus: failed
     */
    int calculate();

public:
    KtAlarmClockParamShared parameter;

private:
    int m_Pretreat;
};

using KtAlarmClockCoreShared = std::shared_ptr<KtAlarmClockCore>;
#endif // KtAlarmClockCore_H
