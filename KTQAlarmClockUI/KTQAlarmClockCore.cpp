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
    , m_Pretreat(KT_E_FAIL) {
    qDebug() << "KTQAlarmClockCore::KTQAlarmClockCore()";
}
//------------------------------------------------
KTQAlarmClockCore::~KTQAlarmClockCore() {
    KTSetNULL(k_pClockParam); // outside
}
//------------------------------------------------
ktErrorCode KTQAlarmClockCore::pretreat() {
    qDebug() << "KTQAlarmClockCore::pretreat()";
    m_Pretreat = KT_E_FAIL;
    if (NULL == k_pClockParam) {
        return m_Pretreat;
    }
    m_Pretreat = KT_E_UNEXPECTED; // unfinish
    return m_Pretreat;
}
//------------------------------------------------
ktErrorCode KTQAlarmClockCore::calculate() {
    qDebug() << "KTQAlarmClockCore::calculate()";
    if (m_Pretreat > KT_S_OK) {
        return m_Pretreat;
    }
    return KT_E_UNEXPECTED; // unfinish
}
