#ifndef KtAlarmClockQmlShell_H
#define KtAlarmClockQmlShell_H

#include "KtAlarmClockUI.h"

#include <QObject>
#include <QQmlApplicationEngine>

class KtAlarmClockController;
class KtAlarmClockViewModel;

class ExportedByKtAlarmClockUI KtAlarmClockQmlShell : public QObject {
    Q_OBJECT

public:
    explicit KtAlarmClockQmlShell(KtAlarmClockViewModel* model,
                                  KtAlarmClockController* controller,
                                  QObject* parent = nullptr);

    bool start();
    void retranslate();

private:
    QQmlApplicationEngine  engine_;
    KtAlarmClockViewModel* model_;
    KtAlarmClockController* controller_;
};

#endif // KtAlarmClockQmlShell_H
