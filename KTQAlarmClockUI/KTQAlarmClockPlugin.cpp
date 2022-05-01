#include "KTQAlarmClockPlugin.h"

#include "KTQAlarmClockCmd.h"

#include <qqml.h>

void KTQAlarmClockPlugin::registerTypes(const char *uri) {
    // @uri cn.kuntaisoft.KTQAlarmClockCmd
    qmlRegisterType<KTQAlarmClockCmd>(uri, 1, 0, "KTQAlarmClockCmd");
}
