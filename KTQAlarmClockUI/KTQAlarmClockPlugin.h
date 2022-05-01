/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockPlugin.h
 * @version		V1.0
 * @brief		Plugin class
 */
#ifndef KTQAlarmClockPlugin_H
#define KTQAlarmClockPlugin_H

#include <QQmlExtensionPlugin>

/**
 * @brief Class KTQAlarmClockPlugin
 */
class KTQAlarmClockPlugin : public QQmlExtensionPlugin {
    Q_OBJECT;
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid);

public:
    void registerTypes(const char *uri) override;
};

#endif // KTQAlarmClockPlugin_H
