#ifndef KtAlarmClockViewModel_H
#define KtAlarmClockViewModel_H

#include "KtAlarmClockUI.h"
#include "KtPropertyMacros.h"

#include <QObject>
#include <QString>

class KtAlarmClockParam;

class ExportedByKtAlarmClockUI KtAlarmClockViewModel : public QObject {
    Q_OBJECT

public:
    explicit KtAlarmClockViewModel(QObject* parent = nullptr);

    Q_INVOKABLE QString formatDuration(int seconds) const;

    void set_phase_state(int phaseValue);
    void set_remaining_seconds(int seconds);
    void set_running_state(bool runningValue);
    void sync_from_param(const KtAlarmClockParam& param);

    KT_Q_PROPERTY_AUTO(int, phase)
    KT_Q_PROPERTY_AUTO(bool, running)
    KT_Q_PROPERTY_AUTO(bool, loop)
    KT_Q_PROPERTY_AUTO(bool, forbidden)
    KT_Q_PROPERTY_AUTO(bool, lockVisible)
    KT_Q_PROPERTY_AUTO(int, remainingSeconds)
    KT_Q_PROPERTY_AUTO(QString, remainingText)
    KT_Q_PROPERTY_AUTO(int, workSeconds)
    KT_Q_PROPERTY_AUTO(int, breakSeconds)
    KT_Q_PROPERTY_AUTO(int, forceSeconds)
    KT_Q_PROPERTY_AUTO(bool, settingsVisible)
    KT_Q_PROPERTY_AUTO(QString, locale)
};

#endif // KtAlarmClockViewModel_H
