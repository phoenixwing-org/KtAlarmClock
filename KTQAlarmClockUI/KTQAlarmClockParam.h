/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockParam.h
 * @version		V1.0
 * @brief		parameter class
 */

#ifndef KTQAlarmClockParam_H
#define KTQAlarmClockParam_H

// kt
#include "ktCoreDefine.h"
#include "ktErrorCode.h"

// KTQ
#include "KTQAlarmClockUI.h"

namespace KTQ {

enum SpeechStep {
    SpeechFirst    = 0,
    SpeechDelay    = 1,
    SpeechTalking  = 2,
    SpeechFinished = 3
};
} // namespace KTQ

/**
 * @brief Class KTQAlarmClockParam
 */
class ExportedByKTQAlarmClockUI KTQAlarmClockParam {
public:
    /** @brief Standard constructors and destructors */
    KTQAlarmClockParam();
    virtual ~KTQAlarmClockParam();

    /** @brief Copy constructor and equal operator */
    KTQAlarmClockParam(const KTQAlarmClockParam &);
    KTQAlarmClockParam &operator=(const KTQAlarmClockParam &);

public:
    // clang-format off
    //START KEVIN CAA WIZARD SECTION KTQAlarmClockParam PARAM DECLARATION

    /**
     * @brief ClassTime
     * @author Kevin
     * @note 45 min
     * @id 1
    */
    int ClassTime;

    /**
     * @brief Class Break
     * @author Kevin
     * @note 10 min
     * @id 2
    */
    int ClassBreak;

    /**
     * @brief Punish Time
     * @author Kevin
     * @id 3
    */
    int PunishTime;

    /**
     * @brief Punish Score
     * @author Kevin
     * @id 4
    */
    double PunishScore;

    /**
     * @brief Time Counter
     * @author Kevin
     * @id 5
    */
    int TimeCounter;

    /**
     * @brief Time Step
     * @author Kevin
     * @id 6
    */
    int TimeStep;

    /**
     * @brief Time Total
     * @author Kevin
     * @note total = Work + Break
     * @id 100
    */
    int TimeTotal;

    //END KEVIN CAA WIZARD SECTION KTQAlarmClockParam PARAM DECLARATION

    // clang-format on

public: // functions
    /**
     * @brief set class members to default value
     * @return void
     */
    void clear();

    /**
     * @brief cout class members for debug
     * @return void
     */
    void dump();

    /**
     * @brief set sample value for debug
     * @return void
     */
    void sample();
};
#endif
