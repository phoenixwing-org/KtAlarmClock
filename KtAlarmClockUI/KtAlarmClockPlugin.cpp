/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockPlugin.cpp
 */
// qt
#include <qqml.h>

// Kt
#include "KtAlarmClockDlg.h"
#include "KtAlarmClockPlugin.h"

//--------------------------------------------------------------------
void KtAlarmClockPlugin::registerTypes(const char* uri) {
    // @uri cn.kuntaisoft.KtAlarmClockDlg
    qmlRegisterType<KtAlarmClockDlg>(uri, 1, 0, "KtAlarmClockDlg");
}
