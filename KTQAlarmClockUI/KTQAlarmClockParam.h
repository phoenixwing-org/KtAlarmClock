/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockParam.h
 * @version		V1.0
 * @brief		param define class for AlarmClock
 */

#ifndef KTQAlarmClockParam_H
#define KTQAlarmClockParam_H

// KT
namespace KTQ {

enum SpeechStep { SpeechFirst = 0, SpeechDelay = 1, SpeechTalking = 2, SpeechFinished = 3 };

/**
 * @brief Class AlarmClock
 */
class AlarmClock {
public:
    /** @brief Standard constructors and destructors */
    AlarmClock();
    virtual ~AlarmClock();

    /** @brief Copy constructor and equal operator */
    AlarmClock(const AlarmClock &);
    AlarmClock &operator=(const AlarmClock &);

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
     * @param[in] dumpContents is dump contents
     * @return void
     */
    void dump(bool dumpContents = false);

    /**
     * @brief set sample value for debug
     * @return void
     */
    void sample();
};
} // namespace KTQ
#endif
