/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     Apache-2.0
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
KtAlarmClockParam::KtAlarmClockParam()

    // clang-format off
    //START KEVIN CAA WIZARD SECTION KtAlarmClockParam PARAM CONSTRUCTOR
      : WorkTime(2700),
      WorkBreak(600),
      TimeForce(300),
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
KtAlarmClockParam::KtAlarmClockParam(const KtAlarmClockParam& iOriginal) {
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
    *this = KtAlarmClockParam();
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

    WorkTime = setting.value("WorkTime").toInt();
    if (WorkTime < 60) { // no setting for WorkTime
        this->setDefault();
        return;
    }

    WorkBreak = setting.value("WorkBreak").toInt();
    TimeForce = setting.value("TimeForce").toInt();
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
void KtAlarmClockParam::setWorkTime(int iValue) {
    WorkTime = iValue;
}
//--------------------------------------------------------------------
int KtAlarmClockParam::GetWorkBreak() const {
    return WorkBreak;
}
//--------------------------------------------------------------------
void KtAlarmClockParam::setWorkBreak(int iValue) {
    WorkBreak = iValue;
}
//--------------------------------------------------------------------
int KtAlarmClockParam::GetTimeForce() const {
    return TimeForce;
}
//--------------------------------------------------------------------
void KtAlarmClockParam::setTimeForce(int iValue) {
    TimeForce = iValue;
}
