#include "KtAlarmClockCmd.h"

#include "KtAlarmClock.h"
#include "KtAlarmClockController.h"

#include <QDebug>
#include <QGuiApplication>
#include <QSettings>

#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

KtAlarmClockCmd::KtAlarmClockCmd(QObject* parent)
    : QObject(parent)
    , parameter_(std::make_shared<KtAlarmClockParam>())
    , controller_(nullptr)
    , translationManager_(nullptr)
    , exePath_() {
}

KtAlarmClockCmd::~KtAlarmClockCmd() {
    controller_.clear();
    parameter_.reset();
}

int KtAlarmClockCmd::build() {
    if (controller_)
        return KT_S_OK;

    if (nullptr == qGuiApp)
        return KT_E_INVALIDARG;

    parameter_->load();

    controller_ = new KtAlarmClockController(parameter_, this, translationManager_, this);
    controller_->set_debug_locate(true);
    controller_->start();
    controller_->dispatch_user_action(KtAlarmClock::ActionPlayPause);

    setAutoStart(true);
    return KT_S_OK;
}

void KtAlarmClockCmd::forceQuit() {
    if (qGuiApp)
        qGuiApp->quit();

#ifdef _WIN32
    ::TerminateProcess(::GetCurrentProcess(), 0);
#else
    std::_Exit(0);
#endif
}

int KtAlarmClockCmd::setAutoStart(bool enabled) {
    qDebug() << "Auto Start Path =" << exePath_;
    QSettings reg(QStringLiteral("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),
                  QSettings::NativeFormat);
    if (enabled)
        reg.setValue(QStringLiteral(PROJECT_APP_NAME), exePath_);
    else
        reg.remove(QStringLiteral(PROJECT_APP_NAME));
    return 0;
}
