/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockParam.cpp
 */
// Qt
#include <QDebug>
// std
#include <fstream>
#include <iostream>

// KTQ
#include "KTQAlarmClockParam.h"

//--------------------------------------------------------------------
KTQAlarmClockParam::KTQAlarmClockParam()
    // clang-format off
    //START KEVIN CAA WIZARD SECTION KTQAlarmClockParam PARAM CONSTRUCTOR
    : ClassTime(4500),
      ClassBreak(600),
      PunishTime(0),
      PunishScore(0),
      TimeCounter(0),
      TimeStep(0),
      TimeTotal(5100)
    //END KEVIN CAA WIZARD SECTION KTQAlarmClockParam PARAM CONSTRUCTOR

// clang-format on
{
    qDebug() << "KTQAlarmClockParam::KTQAlarmClockParam()";
}
//--------------------------------------------------------------------
KTQAlarmClockParam::~KTQAlarmClockParam() {
    // clang-format off
    //START KEVIN CAA WIZARD SECTION KTQAlarmClockParam PARAM DESTRUCTOR
    //ClassTime = 4500; //1
    //ClassBreak = 600; //2
    //PunishTime = 0; //3
    //PunishScore = 0; //4
    //TimeCounter = 0; //5
    //TimeStep = 0; //6
    //TimeTotal = 5100; //100
    //END KEVIN CAA WIZARD SECTION KTQAlarmClockParam PARAM DESTRUCTOR

    // clang-format on
}
//--------------------------------------------------------------------
KTQAlarmClockParam::KTQAlarmClockParam(const KTQAlarmClockParam &iOriginal) {
    *this = iOriginal;
}
//--------------------------------------------------------------------
KTQAlarmClockParam &KTQAlarmClockParam::operator=(const KTQAlarmClockParam &iOriginal) {
    // clang-format off
    //START KEVIN CAA WIZARD SECTION KTQAlarmClockParam PARAM EQUAL
    this->ClassTime = iOriginal.ClassTime;
    this->ClassBreak = iOriginal.ClassBreak;
    this->PunishTime = iOriginal.PunishTime;
    this->PunishScore = iOriginal.PunishScore;
    this->TimeCounter = iOriginal.TimeCounter;
    this->TimeStep = iOriginal.TimeStep;
    this->TimeTotal = iOriginal.TimeTotal;
    //END KEVIN CAA WIZARD SECTION KTQAlarmClockParam PARAM EQUAL

    // clang-format on
    return *this;
}
//--------------------------------------------------------------------
void KTQAlarmClockParam::clear() {
    *this = KTQAlarmClockParam();
}
//--------------------------------------------------------------------
void KTQAlarmClockParam::dump() {
    std::cout << " {  ClassTime:" << ClassTime << ",ClassBreak: " << ClassBreak
              << ",PunishTime: " << PunishTime << ",:PunishScore " << PunishScore
              << ",TimeCounter:" << TimeCounter << ", :TimeStep" << TimeStep
              << ",TimeTotal: " << TimeTotal << " }" << std::endl;
}
//--------------------------------------------------------------------
void KTQAlarmClockParam::sample() {
    ClassTime   = 4500; // 1
    ClassBreak  = 600;  // 2
    PunishTime  = 0;    // 3
    PunishScore = 0;    // 4
    TimeCounter = 0;    // 5
    TimeStep    = 0;    // 6
    TimeTotal   = 5100; // 100
}
