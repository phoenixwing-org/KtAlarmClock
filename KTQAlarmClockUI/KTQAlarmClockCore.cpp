/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockCore.cpp
 */
// Qt
#include <QDebug>
// KTQ
#include "KTQAlarmClockCore.h"
#include "KTQAlarmClockParam.h"

//------------------------------------------------
KTQAlarmClockCore::KTQAlarmClockCore()
    : k_pClockParam(NULL)
    , m_Pretreat(1) {
    qDebug() << "KTQAlarmClockCore::KTQAlarmClockCore()";
}
//------------------------------------------------
KTQAlarmClockCore::~KTQAlarmClockCore() {
    k_pClockParam = NULL; // outside
}
//------------------------------------------------
int KTQAlarmClockCore::pretreat() {
    qDebug() << "KTQAlarmClockCore::pretreat()";
    m_Pretreat = 1;
    if (NULL == k_pClockParam) {
        return m_Pretreat;
    }
    m_Pretreat = 2; // unfinish
    return m_Pretreat;
}
//------------------------------------------------
int KTQAlarmClockCore::calculate() {
    qDebug() << "KTQAlarmClockCore::calculate()";
    if (m_Pretreat > 0) {
        return m_Pretreat;
    }
    return 2; // unfinish
}
