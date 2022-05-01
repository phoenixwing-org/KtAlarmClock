#ifndef KTQAlarmClockPlugin_H
#define KTQAlarmClockPlugin_H

#include <QQmlExtensionPlugin>

class KTQAlarmClockPlugin : public QQmlExtensionPlugin {
    Q_OBJECT;
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid);

public:
    void registerTypes(const char *uri) override;
};

#endif // KTQAlarmClockPlugin_H
