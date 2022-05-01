/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockCmd.cpp
 */
// Qt
#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
// KTQ
#include "KTQAlarmClockCmd.h"
#include "KTQAlarmClockDlg.h"
#include "KTQAlarmClockParam.h"

//------------------------------------------------
KTQAlarmClockCmd::KTQAlarmClockCmd(QObject *parent)
    : QObject(parent)
    , m_pClockParam(NULL)
    , m_pClockDlg(NULL) {

    m_pClockParam = new KTQAlarmClockParam();
}
//------------------------------------------------
KTQAlarmClockCmd::~KTQAlarmClockCmd() {
    // delete
    delete m_pClockParam;
    m_pClockParam = NULL;
    m_pClockDlg   = NULL;
}
//------------------------------------------------
int KTQAlarmClockCmd::BuildDialog(QQmlApplicationEngine *engine) {

    qDebug() << "KTQAlarmClockCmd::BuildDialog()";

    if (nullptr == qGuiApp) {
        return 1;
    } else if (nullptr == engine) {
        return 1;
    }

    // m_pClockDlg = new KTQAlarmClockDlg();

    const QUrl url(QStringLiteral("qrc:/MyMain.qml"));
    QObject::connect(
        engine, &QQmlApplicationEngine::objectCreated, qGuiApp,
        [ url ](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine->load(url);
    return 0;
}
//------------------------------------------------
void KTQAlarmClockCmd::debug(const QString &iMsg) {

    qDebug() << "Hello to KTQAlarmClockCmd. msg = " << iMsg;
}
