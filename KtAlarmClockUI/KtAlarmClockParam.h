/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockParam.h
 * @version		V1.0
 * @brief		parameter class
 */

#ifndef KtAlarmClockParam_H
#define KtAlarmClockParam_H

#include <memory>

// Kt
#include "KtAlarmClock.h"
#include "KtAlarmClockUI.h"

/**
 * @brief Version of KtAlarmClock
 */
#define VERSION_KtAlarmClock "3.1"

/**
 * @brief Class KtAlarmClockParam
 */
class ExportedByKtAlarmClockUI KtAlarmClockParam {

public:
    /** @brief Standard constructors and destructors */
    KtAlarmClockParam();
    virtual ~KtAlarmClockParam();

    /** @brief Copy constructor and equal operator */
    KtAlarmClockParam(const KtAlarmClockParam&);
    KtAlarmClockParam& operator=(const KtAlarmClockParam&);

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
     * @brief Get Time Counter
     */
    int GetTimeForce() const;

    /**
     * @brief Get Work Break
     */
    int GetWorkBreak() const;

    /**
     * @brief Get Work Time
     */
    int GetWorkTime() const;

    /**
     * @brief Register Read from registry
     * 
     */
    void registerRead();

    /**
     * @brief Register Write to registry
     */
    void registerWrite();

    /**
     * @brief set default value for debug
     * @return void
     */
    void setDefault();

    /**
     * @brief Set Time Counter
     */
    void setTimeForce(int iValue);

    /**
     * @brief Set Work Break
     */
    void setWorkBreak(int iValue);

    /**
     * @brief Set Work Time
     */
    void setWorkTime(int iValue);

public:
    // clang-format off
    //START KEVIN CAA WIZARD SECTION KtAlarmClockParam PARAM DECLARATION

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
     * @brief Time Force
     * @author Kevin
     * @id 4
    */
    int TimeForce;

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
    int WorkStep;

    /**
     * @brief Time Counter
     * @author Kevin
     * @id 102
    */
    int TimeCounter;

    //END KEVIN CAA WIZARD SECTION KtAlarmClockParam PARAM DECLARATION

    // clang-format on
};

/** @brief Shared pointer to KtAlarmClockParam */
using KtAlarmClockParamShared = std::shared_ptr<KtAlarmClockParam>;
#endif
