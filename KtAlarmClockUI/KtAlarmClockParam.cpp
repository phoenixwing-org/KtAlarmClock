/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockParam.cpp
 */
// Qt
#include <QDebug>
#include <QSettings>

// std
#include <fstream>
#include <iostream>

// Kt
#include "KtAlarmClockParam.h"

//--------------------------------------------------------------------
KtAlarmClockParam::KtAlarmClockParam(QObject* parent)
    : QObject(parent)
    ,
    // clang-format off
    //START KEVIN CAA WIZARD SECTION KtAlarmClockParam PARAM CONSTRUCTOR
      WorkTime(2700),
      WorkBreak(600),
      TimeForce(600),
      TimeTotal(5100),
      WorkStep(1),
      TimeCounter(0)
    //END KEVIN CAA WIZARD SECTION KtAlarmClockParam PARAM CONSTRUCTOR

// clang-format on
{
    // qDebug() << "KtAlarmClockParam::KtAlarmClockParam()";
}
//--------------------------------------------------------------------
KtAlarmClockParam::~KtAlarmClockParam() {
    // clang-format off
    //START KEVIN CAA WIZARD SECTION KtAlarmClockParam PARAM DESTRUCTOR
    //WorkTime = 2700; //2
    //WorkBreak = 600; //3
    //TimeForce = 600; //4
    //TimeTotal = 5100; //100
    //WorkStep = 1; //101
    //TimeCounter = 0; //102
    //END KEVIN CAA WIZARD SECTION KtAlarmClockParam PARAM DESTRUCTOR

    // clang-format on
}
//--------------------------------------------------------------------
KtAlarmClockParam::KtAlarmClockParam(const KtAlarmClockParam& iOriginal)
    : QObject(iOriginal.parent()) {
    *this = iOriginal;
}
//--------------------------------------------------------------------
KtAlarmClockParam& KtAlarmClockParam::operator=(const KtAlarmClockParam& iOriginal) {
    // clang-format off
    //START KEVIN CAA WIZARD SECTION KtAlarmClockParam PARAM EQUAL
    this->WorkTime = iOriginal.WorkTime;
    this->WorkBreak = iOriginal.WorkBreak;
    this->TimeForce = iOriginal.TimeForce;
    this->TimeTotal = iOriginal.TimeTotal;
    this->WorkStep = iOriginal.WorkStep;
    this->TimeCounter = iOriginal.TimeCounter;
    //END KEVIN CAA WIZARD SECTION KtAlarmClockParam PARAM EQUAL

    // clang-format on
    return *this;
}
//--------------------------------------------------------------------
void KtAlarmClockParam::clear() {
    *this = KtAlarmClockParam();
}
//--------------------------------------------------------------------
void KtAlarmClockParam::dump() {
    std::cout << "{\"WorkTime\":" << WorkTime << ",\"WorkBreak\":" << WorkBreak
              << ",\"TimeForce\": " << TimeForce << ", \"TimeStep\":" << WorkStep
              << ",\"TimeTotal\": " << TimeTotal << " }" << std::endl;
}
//--------------------------------------------------------------------
void KtAlarmClockParam::setDefault() {
    WorkTime  = 2700; // 1
    WorkBreak = 600;  // 2
    TimeForce = 600;  // 3
    WorkStep  = 0;    // 6
    TimeTotal = 3300; // 100
}
//--------------------------------------------------------------------
int KtAlarmClockParam::GetWorkTime() const {
    return WorkTime;
}
//--------------------------------------------------------------------
void KtAlarmClockParam::registerRead() {
    qDebug() << "KtAlarmClockParam::registerRead() ";
    QSettings setting("HKEY_CURRENT_USER\\SOFTWARE\\KuntaiSoft\\KtAlarmClock",
                      QSettings::NativeFormat); // open

    WorkBreak = setting.value("WorkBreak").toInt();
    TimeForce = setting.value("TimeForce").toInt();
    WorkTime  = setting.value("WorkTime").toInt();
}
//--------------------------------------------------------------------
void KtAlarmClockParam::registerWrite() {
    qDebug() << "KtAlarmClockParam::registerWrite() ";
    QSettings setting("HKEY_CURRENT_USER\\SOFTWARE\\KuntaiSoft\\KtAlarmClock",
                      QSettings::NativeFormat); // open

    setting.setValue("WorkBreak", WorkBreak);
    setting.setValue("TimeForce", TimeForce);
    setting.setValue("WorkTime", WorkTime);
}
//--------------------------------------------------------------------
void KtAlarmClockParam::SetWorkTime(int iValue) {
    if (WorkTime != iValue) {
        WorkTime = iValue;
        emit sigWorkTime(iValue);
    }
}
//--------------------------------------------------------------------
int KtAlarmClockParam::GetWorkBreak() const {
    return WorkBreak;
}
//--------------------------------------------------------------------
void KtAlarmClockParam::SetWorkBreak(int iValue) {
    if (WorkBreak != iValue) {
        WorkBreak = iValue;
        emit sigWorkBreak(iValue);
    }
}
//--------------------------------------------------------------------
int KtAlarmClockParam::GetTimeForce() const {
    return TimeForce;
}
//--------------------------------------------------------------------
void KtAlarmClockParam::SetTimeForce(int iValue) {
    if (TimeForce != iValue) {
        TimeForce = iValue;
        emit sigTimeForce(iValue);
    }
}
