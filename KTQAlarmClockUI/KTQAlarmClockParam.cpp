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
KTQAlarmClockParam::KTQAlarmClockParam(QObject* parent)
    : QObject(parent)
    ,
    // clang-format off
    //START KEVIN CAA WIZARD SECTION KTQAlarmClockParam PARAM CONSTRUCTOR
      WorkTime(2700),
      WorkBreak(600),
      PunishTime(0),
      PunishScore(0),
      TimeTotal(5100),
      WorkStep(KTQ::WorkStepNone),
      TimeCounter(0)
    //END KEVIN CAA WIZARD SECTION KTQAlarmClockParam PARAM CONSTRUCTOR

// clang-format on
{
    qDebug() << "KTQAlarmClockParam::KTQAlarmClockParam()";
}
//--------------------------------------------------------------------
KTQAlarmClockParam::~KTQAlarmClockParam() {
    // clang-format off
    //START KEVIN CAA WIZARD SECTION KTQAlarmClockParam PARAM DESTRUCTOR
    //WorkTime = 2700; //2
    //WorkBreak = 600; //3
    //PunishTime = 0; //4
    //PunishScore = 0; //5
    //TimeTotal = 5100; //100
    //WorkStep = KTQ::WorkStepNone; //101
    //TimeCounter = 0; //102
    //END KEVIN CAA WIZARD SECTION KTQAlarmClockParam PARAM DESTRUCTOR

    // clang-format on
}
//--------------------------------------------------------------------
KTQAlarmClockParam::KTQAlarmClockParam(const KTQAlarmClockParam& iOriginal)
    : QObject(iOriginal.parent()) {
    *this = iOriginal;
}
//--------------------------------------------------------------------
KTQAlarmClockParam& KTQAlarmClockParam::operator=(const KTQAlarmClockParam& iOriginal) {
    // clang-format off
    //START KEVIN CAA WIZARD SECTION KTQAlarmClockParam PARAM EQUAL
    this->WorkTime = iOriginal.WorkTime;
    this->WorkBreak = iOriginal.WorkBreak;
    this->PunishTime = iOriginal.PunishTime;
    this->PunishScore = iOriginal.PunishScore;
    this->TimeTotal = iOriginal.TimeTotal;
    this->WorkStep = iOriginal.WorkStep;
    this->TimeCounter = iOriginal.TimeCounter;
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
    std::cout << " {  WorkTime:" << WorkTime << ",WorkBreak: " << WorkBreak
              << ",PunishTime: " << PunishTime << ",:PunishScore " << PunishScore
              << ",TimeCounter:" << TimeCounter << ", :TimeStep" << WorkStep
              << ",TimeTotal: " << TimeTotal << " }" << std::endl;
}
//--------------------------------------------------------------------
void KTQAlarmClockParam::sample() {
    WorkTime    = 4500;              // 1
    WorkBreak   = 600;               // 2
    PunishTime  = 0;                 // 3
    PunishScore = 0;                 // 4
    TimeCounter = 0;                 // 5
    WorkStep    = KTQ::WorkStepNone; // 6
    TimeTotal   = 5100;              // 100
}
//--------------------------------------------------------------------
int KTQAlarmClockParam::GetWorkTime() const {
    return WorkTime;
}
//--------------------------------------------------------------------
void KTQAlarmClockParam::SetWorkTime(int iValue) {
    if (WorkTime != iValue) {
        WorkTime = iValue;
        emit sigWorkTime(iValue);
    }
}
//--------------------------------------------------------------------
int KTQAlarmClockParam::GetWorkBreak() const {
    return WorkBreak;
}
//--------------------------------------------------------------------
void KTQAlarmClockParam::SetWorkBreak(int iValue) {
    if (WorkBreak != iValue) {
        WorkBreak = iValue;
        emit sigWorkBreak(iValue);
    }
}
//--------------------------------------------------------------------
int KTQAlarmClockParam::GetTimeCounter() const {
    return TimeCounter;
}
//--------------------------------------------------------------------
void KTQAlarmClockParam::SetTimeCounter(int iValue) {
    if (TimeCounter != iValue) {
        TimeCounter = iValue;
        emit sigTimeCounter(iValue);
    }
}
