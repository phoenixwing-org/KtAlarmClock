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
#include "KTQAlarmClockCore.h"
#include "KTQAlarmClockDlg.h"
#include "KTQAlarmClockParam.h"

//------------------------------------------------
KTQAlarmClockCmd::KTQAlarmClockCmd(QObject *parent)
    : QObject(parent)
    , m_pClockCore(NULL)
    , m_pClockParam(NULL)
    , m_pClockDlg(NULL) {
    qDebug() << "KTQAlarmClockCmd::KTQAlarmClockCmd()";
    // new
    m_pClockParam = new KTQAlarmClockParam();
    m_pClockCore  = new KTQAlarmClockCore();
    // set value
    m_pClockCore->k_pClockParam = m_pClockParam;
}
//------------------------------------------------
KTQAlarmClockCmd::~KTQAlarmClockCmd() {
    qDebug() << "KTQAlarmClockCmd::~KTQAlarmClockCmd()";
    // delete
    KTDelete(m_pClockParam);
    KTDelete(m_pClockCore);

    // only set NULL
    KTSetNULL(m_pClockDlg);
}
//------------------------------------------------
ktErrorCode KTQAlarmClockCmd::BuildDialog(QQmlApplicationEngine *engine) {
    qDebug() << "KTQAlarmClockCmd::BuildDialog()";

    m_pClockParam = new KTQAlarmClockParam();
    if (nullptr == qGuiApp) {
        return KT_E_INVALIDARG;
    }
    else if (nullptr == engine) {
        return KT_E_INVALIDARG;
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
    return KT_S_OK;
}
//------------------------------------------------
void KTQAlarmClockCmd::debug(const QString &iMsg) {
    qDebug() << "Hello to KTQAlarmClockCmd. msg = " << iMsg;
}
