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

#include <QObject>
// kt
#include "ktCoreDefine.h"
#include "ktErrorCode.h"

// Kt
#include "KtAlarmClock.h"
#include "KtAlarmClockUI.h"

/**
 * @brief Class KtAlarmClockParam
 */
class ExportedByKtAlarmClockUI KtAlarmClockParam : public QObject {
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
     * @brief Time Force Property
     */
    Q_PROPERTY(int TimeForce READ GetTimeForce WRITE SetTimeForce NOTIFY sigTimeForce);

public:
    /** @brief Standard constructors and destructors */
    KtAlarmClockParam(QObject* parent = nullptr);
    virtual ~KtAlarmClockParam();

    /** @brief Copy constructor and equal operator */
    KtAlarmClockParam(const KtAlarmClockParam&);
    KtAlarmClockParam& operator=(const KtAlarmClockParam&);

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
    int GetTimeForce() const;

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
    bool sigTimeForce(int iValue);

    /**
     * @brief Show sub dialog
     */
    bool sigDialogShow(int index, bool value);

    /**
     * @brief clock timeout
     */
    bool sigClockOut(int state);

    /**
     * @brief clock state
     */
    bool sigClockStart(int state);

    /**
     * @brief Action state
     */
    bool sigAction(int state);

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
    void SetTimeForce(int iValue);

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
