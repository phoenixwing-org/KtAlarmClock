/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockCore.cpp
 */
// Qt
#include <QDebug>
// Kt
#include "KtAlarmClockCore.h"
#include "KtAlarmClockParam.h"

//------------------------------------------------
KtAlarmClockCore::KtAlarmClockCore()
    : k_pClockParam(NULL)
    , m_Pretreat(KT_E_FAIL) {
    // qDebug() << "KtAlarmClockCore::KtAlarmClockCore()";
}
//------------------------------------------------
KtAlarmClockCore::~KtAlarmClockCore() {
    KTSetNULL(k_pClockParam); // outside
}
//------------------------------------------------
int KtAlarmClockCore::pretreat() {
    qDebug() << "KtAlarmClockCore::pretreat()";
    m_Pretreat = KT_E_FAIL;
    if (NULL == k_pClockParam) {
        return m_Pretreat;
    }
    m_Pretreat = KT_E_UNEXPECTED; // unfinish
    return m_Pretreat;
}
//------------------------------------------------
int KtAlarmClockCore::calculate() {
    qDebug() << "KtAlarmClockCore::calculate()";
    if (m_Pretreat > KT_S_OK) {
        return m_Pretreat;
    }
    return KT_E_UNEXPECTED; // unfinish
}
