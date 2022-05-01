/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockPlugin.cpp
 */
// qt
#include <qqml.h>

// KTQ
#include "KTQAlarmClockDlg.h"
#include "KTQAlarmClockPlugin.h"

//--------------------------------------------------------------------
void KTQAlarmClockPlugin::registerTypes(const char *uri) {
    // @uri cn.kuntaisoft.KTQAlarmClockDlg
    qmlRegisterType<KTQAlarmClockDlg>(uri, 1, 0, "KTQAlarmClockDlg");
}
