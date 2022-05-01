/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockCmd.cpp
 */

#include "KTQAlarmClockCmd.h"
#include "KTQAlarmClockDlg.h"
#include "KTQAlarmClockParam.h"

//------------------------------------------------
KTQAlarmClockCmd::KTQAlarmClockCmd(QObject *parent)
    : QObject(parent)
    , m_pClockParam(NULL)
    , m_pClockDlg(NULL) {

    m_pClockParam = new KTQAlarmClockParam();
    // m_pClockDlg   = new KTQAlarmClockDlg();
}
//------------------------------------------------
KTQAlarmClockCmd::~KTQAlarmClockCmd() {
    // delete
    delete m_pClockParam;
    m_pClockParam = NULL;
    m_pClockDlg   = NULL;
}
