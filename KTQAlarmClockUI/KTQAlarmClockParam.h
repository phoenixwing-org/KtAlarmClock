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

#include <QObject>
// kt
#include "ktCoreDefine.h"
#include "ktErrorCode.h"

// KTQ
#include "KTQAlarmClockUI.h"

namespace KTQ {

/**
 * @brief Work Step
 */
enum WorkStep {
    WorkStepNone   = 0,
    WorkStepDoing  = 1,
    WorkStepDelay  = 2,
    WorkStepRest   = 3,
    WorkStepFinish = 4
};
} // namespace KTQ

/**
 * @brief Class KTQAlarmClockParam
 */
class ExportedByKTQAlarmClockUI KTQAlarmClockParam : public QObject {
    Q_OBJECT;

    /**
     * @brief Work Time Property
     */
    Q_PROPERTY(int WorkTime READ GetWorkTime WRITE SetWorkTime NOTIFY sigWorkTime);

    /**
     * @brief Work Break Property
     */
    Q_PROPERTY(int WorkBreak READ GetWorkBreak WRITE SetWorkBreak NOTIFY sigWorkBreak);

    /**
     * @brief Time Counter Property
     */
    Q_PROPERTY(int TimeCounter READ GetTimeCounter NOTIFY sigTimeCounter);

public:
    /** @brief Standard constructors and destructors */
    KTQAlarmClockParam(QObject* parent = nullptr);
    virtual ~KTQAlarmClockParam();

    /** @brief Copy constructor and equal operator */
    KTQAlarmClockParam(const KTQAlarmClockParam&);
    KTQAlarmClockParam& operator=(const KTQAlarmClockParam&);

public:
    /**
     * @brief Get Work Time
     */
    int GetWorkTime() const;

    /**
     * @brief Get Work Break
     */
    int GetWorkBreak() const;

    /**
     * @brief Get Time Counter
     */
    int GetTimeCounter() const;

signals:
    /**
     * @brief Work Time Signal
     */
    bool sigWorkTime(int iValue);

    /**
     * @brief Work Break Signal
     */
    bool sigWorkBreak(int iValue);

    /**
     * @brief Time Counter Signal
     */
    bool sigTimeCounter(int iValue);

public slots:
    /**
     * @brief Set Work Time
     */
    void SetWorkTime(int iValue);

    /**
     * @brief Set Work Break
     */
    void SetWorkBreak(int iValue);

    /**
     * @brief Set Time Counter
     */
    void SetTimeCounter(int iValue);

public:
    // clang-format off
    //START KEVIN CAA WIZARD SECTION KTQAlarmClockParam PARAM DECLARATION

    /**
     * @brief Work Time
     * @author Kevin
     * @note 45 min
     * @id 2
    */
    int WorkTime;

    /**
     * @brief Work Break
     * @author Kevin
     * @note 10 min
     * @id 3
    */
    int WorkBreak;

    /**
     * @brief Punish Time
     * @author Kevin
     * @id 4
    */
    int PunishTime;

    /**
     * @brief Punish Score
     * @author Kevin
     * @id 5
    */
    double PunishScore;

    /**
     * @brief Time Total
     * @author Kevin
     * @note total = Work + Break
     * @id 100
    */
    int TimeTotal;

    /**
     * @brief Work Step
     * @author Kevin
     * @id 101
    */
    KTQ::WorkStep WorkStep;

    /**
     * @brief Time Counter
     * @author Kevin
     * @id 102
    */
    int TimeCounter;

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
