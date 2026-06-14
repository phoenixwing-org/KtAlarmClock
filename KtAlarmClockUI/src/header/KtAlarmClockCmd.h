#ifndef KtAlarmClockCmd_H
#define KtAlarmClockCmd_H

#include "KtAlarmClockParam.h"
#include "KtAlarmClockUI.h"

#include <QObject>
#include <QPointer>
#include <QString>

class KtAlarmClockController;
class KtTranslationManager;

class ExportedByKtAlarmClockUI KtAlarmClockCmd : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(KtAlarmClockCmd)

public:
    explicit KtAlarmClockCmd(QObject* parent = nullptr);
    ~KtAlarmClockCmd() override;

    int build();

    void setExePath(const QString& path) {
        exePath_ = path;
    }

    void setTranslationManager(KtTranslationManager* manager) {
        translationManager_ = manager;
    }

public slots:
    void forceQuit();
    int setAutoStart(bool enabled);

private:
    KtAlarmClockParamShared parameter_;
    QPointer<KtAlarmClockController> controller_;
    KtTranslationManager* translationManager_;
    QString exePath_;
};

#endif // KtAlarmClockCmd_H
