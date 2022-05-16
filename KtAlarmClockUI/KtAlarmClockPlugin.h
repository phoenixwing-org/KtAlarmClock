/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockPlugin.h
 * @version		V1.0
 * @brief		Plugin class
 */
#ifndef KtAlarmClockPlugin_H
#define KtAlarmClockPlugin_H

#include <QQmlExtensionPlugin>

/**
 * @brief Class KtAlarmClockPlugin
 */
class KtAlarmClockPlugin : public QQmlExtensionPlugin {
    Q_OBJECT;
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid);

public:
    void registerTypes(const char* uri) override;
};

#endif // KtAlarmClockPlugin_H
