/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockCmd.cpp
 */
// Qt
#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSettings>

// Kt
#include "KtAlarmClock.h"
#include "KtAlarmClockCmd.h"
#include "KtAlarmClockCore.h"
#include "KtAlarmClockDlg.h"
#include "KtAlarmClockParam.h"

//------------------------------------------------
KtAlarmClockCmd::KtAlarmClockCmd(QObject* parent)
    : QObject(parent)
    , m_pClockCore(NULL)
    , m_pClockParam(NULL)
    , m_pClockDlg(NULL)
    , m_ExePath() {
    qDebug() << "KtAlarmClockCmd::KtAlarmClockCmd()";
    // new
    m_pClockParam = new KtAlarmClockParam();
    m_pClockCore  = new KtAlarmClockCore();
    // set value
    m_pClockCore->k_pClockParam = m_pClockParam;
    qmlRegisterType<KtAlarmClock>("KtAlarmClock", 1, 0, "Kt");
}
//------------------------------------------------
KtAlarmClockCmd::~KtAlarmClockCmd() {
    qDebug() << "KtAlarmClockCmd::~KtAlarmClockCmd()";
    // delete
    KTDelete(m_pClockParam);
    KTDelete(m_pClockCore);

    // only set NULL
    KTSetNULL(m_pClockDlg);
}
//------------------------------------------------
ktErrorCode KtAlarmClockCmd::BuildDialog(QQmlApplicationEngine* engine) {
    qDebug() << "KtAlarmClockCmd::BuildDialog()";
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
    m_pClockDlg = new KtAlarmClockDlg();

    // register m_pClockParam to qml
    engine->rootContext()->setContextProperty("myAlarmClockParam", m_pClockParam);
    // register command to qml
    engine->rootContext()->setContextProperty("myAlarmClockCmd", this);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
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
void KtAlarmClockCmd::debug(const QString& iMsg) {
    qDebug() << "Hello to KtAlarmClockCmd. msg = " << iMsg;
}
//------------------------------------------------
QQuickItem* KtAlarmClockCmd::GiveMyPanel() const {
    return m_pClockDlg;
}
//------------------------------------------------
int KtAlarmClockCmd::SetAutoStart(bool iValue) {
    qDebug() << "KtAlarmClockCmd::SetAutoStart" << iValue;
    qDebug() << "Auto Start Path = " << m_ExePath;
    QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                  QSettings::NativeFormat);

    reg.setValue("KtAlarmClock", m_ExePath);
    return 0; // ok
}
//------------------------------------------------
int KtAlarmClockCmd::onStart() {
    qDebug() << "KtAlarmClockCmd::onStart()";
    emit m_pClockParam->sigDialogShow(KtAlarmClock::DlgBreak, false);
    emit m_pClockParam->sigDialogShow(KtAlarmClock::DlgMain, false);

    emit m_pClockParam->sigClockStart(KtAlarmClock::WorkTime);

    // m_pClockParam->SetTimeCounter(m_pClockParam->TimeCounter + 60);
    return KT_S_OK;
}
