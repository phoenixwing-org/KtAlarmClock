#include "KtAlarmClockQmlShell.h"

#include "KtAlarmClock.h"
#include "KtAlarmClockController.h"
#include "KtAlarmClockViewModel.h"

#include <QDebug>
#include <QCoreApplication>
#include <QQmlContext>
#include <QtQml/qqml.h>

//----------------------------------------
KtAlarmClockQmlShell::KtAlarmClockQmlShell(KtAlarmClockViewModel* model,
                                           KtAlarmClockController* controller,
                                           QObject* parent)
    : QObject(parent)
    , engine_(this)
    , model_(model)
    , controller_(controller) {
}
//----------------------------------------
void KtAlarmClockQmlShell::retranslate() {
    engine_.retranslate();
}
//----------------------------------------
bool KtAlarmClockQmlShell::start() {
    if (!model_ || !controller_)
        return false;

    qmlRegisterUncreatableType<KtAlarmClock>("KtAlarmClockQml", 1, 0, "KtAlarmClock",
                                             QStringLiteral("Enums only"));

    const QString appDir = QCoreApplication::applicationDirPath();
    engine_.addImportPath(appDir + QStringLiteral("/qml"));
    engine_.addImportPath(appDir);
    engine_.addImportPath(QStringLiteral("qrc:/qt/qml"));

    engine_.rootContext()->setContextProperty(QStringLiteral("alarmModel"), model_);
    engine_.rootContext()->setContextProperty(QStringLiteral("alarmController"), controller_);
    engine_.loadFromModule(QStringLiteral("KtAlarmClockQml"), QStringLiteral("Main"));

    const bool loaded = !engine_.rootObjects().isEmpty();
    if (!loaded)
        qWarning() << "[QML] failed to load Main";
    return loaded;
}
