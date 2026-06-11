/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockCmd.cpp
 */
// Qt
#include <QDebug>
#include <QGuiApplication>
#include <QSettings>

// std
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

// Kt
#include "KtAlarmClock.h"
#include "KtAlarmClockCmd.h"
#include "KtAlarmClockController.h"
#include "KtAlarmClockCore.h"
#include "KtAlarmClockDlg.h"
#include "KtAlarmClockParam.h"
#include "KtSingleInstanceGuard.h"

//------------------------------------------------
KtAlarmClockCmd::KtAlarmClockCmd(QObject* parent)
    : QObject(parent)
    , core(nullptr)
    , parameter(nullptr)
    , dialog(nullptr)
    , m_pController(nullptr)
    , instanceGuard_(nullptr)
    , m_ExePath() {
    // qDebug() << "KtAlarmClockCmd::KtAlarmClockCmd()";
    //  new
    parameter = std::make_shared<KtAlarmClockParam>();
    core      = std::make_shared<KtAlarmClockCore>();
    // set value
    core->parameter = parameter;
}
//------------------------------------------------
KtAlarmClockCmd::~KtAlarmClockCmd() {
    // qDebug() << "KtAlarmClockCmd::~KtAlarmClockCmd()";
    //  delete
    parameter = nullptr;
    core      = nullptr;
    KTDelete(m_pController);
    KTDelete(instanceGuard_);

    // only set NULL
    KTSetNULL(dialog);
}
//------------------------------------------------
int KtAlarmClockCmd::build() {
    // qDebug() << "KtAlarmClockCmd::build()";
    if (NULL != m_pController) {
        return KT_S_OK;
    }

    if (nullptr == qGuiApp) {
        return KT_E_INVALIDARG;
    }

    instanceGuard_ = new KtSingleInstanceGuard(this);
    if (!instanceGuard_->try_acquire_primary()) return KT_S_ALREADY_RUNNING;

    // dialog
    dialog = new KtAlarmClockDlg();

    parameter->registerRead();

    m_pController = new KtAlarmClockController(parameter, this);
#ifndef NDEBUG
    m_pController->set_debug_locate(true);
#endif
    m_pController->start();

    connect(instanceGuard_, &KtSingleInstanceGuard::activate_requested, m_pController,
            &KtAlarmClockController::on_second_instance_activate);
    instanceGuard_->start_listening();

    m_pController->dispatch_user_action(KtAlarmClock::ActionPlayPause);

    // auto start
    setAutoStart(true);

    return KT_S_OK;
}
//------------------------------------------------
void KtAlarmClockCmd::debug(const QString& iMsg) {
    qDebug() << "Hello to KtAlarmClockCmd. msg = " << iMsg;
}
//------------------------------------------------
KtAlarmClockDlg* KtAlarmClockCmd::giveMyPanel() const {
    return dialog;
}
//------------------------------------------------
int KtAlarmClockCmd::setAutoStart(bool iValue) {
    // qDebug() << "KtAlarmClockCmd::setAutoStart" << iValue;
#ifndef NDEBUG
    qDebug() << "Auto Start Path = " << m_ExePath;
#endif
    QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                  QSettings::NativeFormat);

    reg.setValue("KtAlarmClock", m_ExePath);
    return 0; // ok
}
//------------------------------------------------
void KtAlarmClockCmd::forceQuit() {
    if (qGuiApp) qGuiApp->quit();

#ifdef _WIN32
    ::TerminateProcess(::GetCurrentProcess(), 0);
#else
    std::_Exit(0);
#endif
}
