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
#include <QQmlContext>
// KTQ
#include "KTQAlarmClockCmd.h"
#include "KTQAlarmClockCore.h"
#include "KTQAlarmClockDlg.h"
#include "KTQAlarmClockParam.h"

//------------------------------------------------
KTQAlarmClockCmd::KTQAlarmClockCmd(QObject* parent)
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
ktErrorCode KTQAlarmClockCmd::BuildDialog(QQmlApplicationEngine* engine) {
    qDebug() << "KTQAlarmClockCmd::BuildDialog()";
    if (NULL != m_pClockDlg) {
        return KT_S_OK;
    }

    if (nullptr == qGuiApp) {
        return KT_E_INVALIDARG;
    }
    else if (nullptr == engine) {
        return KT_E_INVALIDARG;
    }
    // m_pClockDlg
    m_pClockDlg = new KTQAlarmClockDlg();

    // register m_pClockParam to qml
    engine->rootContext()->setContextProperty("myAlarmClockParam", m_pClockParam);
    // register command to qml
    engine->rootContext()->setContextProperty("myAlarmClockCmd", this);

    const QUrl url(QStringLiteral("qrc:/MyMain.qml"));
    /*
     * What's the meaning?
    QObject::connect(
        engine, &QQmlApplicationEngine::objectCreated, qGuiApp,
        [ url ](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl) QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    */
    engine->load(url);

    return KT_S_OK;
}
//------------------------------------------------
void KTQAlarmClockCmd::debug(const QString& iMsg) {
    qDebug() << "Hello to KTQAlarmClockCmd. msg = " << iMsg;
}
//------------------------------------------------
QQuickItem* KTQAlarmClockCmd::GiveMyPanel() const {
    return m_pClockDlg;
}
//------------------------------------------------
int KTQAlarmClockCmd::onStart() {
    qDebug() << "KTQAlarmClockCmd::onStart()";
    m_pClockParam->SetTimeCounter(m_pClockParam->TimeCounter + 60);
    return KT_S_OK;
}
