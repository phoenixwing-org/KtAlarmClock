#include "KtAlarmClockController.h"

#include "KtAlarmClock.h"
#include "KtAlarmClockCmd.h"
#include "KtAlarmClockParam.h"
#include "KtAlarmClockQmlShell.h"
#include "KtAlarmClockSettings.h"
#include "KtAlarmClockViewModel.h"
#include "KtClockRuntime.h"
#include "KtLockScreenManager.h"
#include "KtTranslationManager.h"
#include "KtTrayService.h"

#include <QCursor>
#include <QDebug>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QPoint>
#include <QRect>
#include <QScreen>
#include <QSize>
#include <QTimer>
#include <QUrl>
#include <QVariantMap>

namespace {
constexpr int kMinuteStepSeconds = 60;

bool lock_debug_layout_enabled() {
    bool ok = false;
    const int enabled = qEnvironmentVariableIntValue("KT_ALARM_CLOCK_LOCK_DEBUG", &ok);
    return ok && enabled != 0;
}

QVariantMap clamp_to_screen(QScreen* screen, int globalX, int globalY, const QSize& windowSize) {
    if (!screen)
        return {{QStringLiteral("x"), globalX}, {QStringLiteral("y"), globalY}};

    const QRect available = screen->availableGeometry();
    int x = globalX;
    int y = globalY;

    if (x < available.left())
        x = available.left();
    if (y < available.top())
        y = available.top();
    if (x + windowSize.width() > available.right() + 1)
        x = available.right() + 1 - windowSize.width();
    if (y + windowSize.height() > available.bottom() + 1)
        y = available.bottom() + 1 - windowSize.height();

    if (windowSize.width() > available.width())
        x = available.left();
    if (windowSize.height() > available.height())
        y = available.top();

    return {{QStringLiteral("x"), x}, {QStringLiteral("y"), y}};
}
}

//----------------------------------------
KtAlarmClockController::KtAlarmClockController(KtAlarmClockParamShared param, KtAlarmClockCmd* cmd,
                                               KtTranslationManager* translationManager,
                                               QObject* parent)
    : QObject(parent)
    , parameter(param)
    , cmd_(cmd)
    , translationManager_(translationManager)
    , runtime_(nullptr)
    , tray_(nullptr)
    , lockScreen_(nullptr)
    , model_(new KtAlarmClockViewModel(this))
    , qmlShell_(nullptr)
    , loop_(false)
    , phase_(KtAlarmClock::None)
    , debugLocate_(true)
    , lockDebugMode_(lock_debug_layout_enabled()) {
    model_->sync_from_param(*parameter);
    model_->locale(KtAlarmClockSettings::read_locale());
}
//----------------------------------------
KtAlarmClockController::~KtAlarmClockController() {
    delete qmlShell_;
    qmlShell_ = nullptr;
}
//----------------------------------------
void KtAlarmClockController::adjustRemaining(int deltaSeconds) {
    if (is_forbidden())
        return;
    handle_time_adjustment(deltaSeconds);
    sync_model();
}
//----------------------------------------
QVariantMap KtAlarmClockController::clampWindowPosition(int globalX, int globalY, int width,
                                                        int height) const {
    const QSize windowSize(qMax(1, width), qMax(1, height));
    const QPoint topLeft(globalX, globalY);
    const QPoint center(globalX + windowSize.width() / 2, globalY + windowSize.height() / 2);

    QScreen* screen = QGuiApplication::screenAt(center);
    if (!screen)
        screen = QGuiApplication::screenAt(topLeft);
    if (!screen)
        screen = QGuiApplication::primaryScreen();

    return clamp_to_screen(screen, globalX, globalY, windowSize);
}
//----------------------------------------
QVariantMap KtAlarmClockController::clampWindowPositionOnCurrentScreen(
    int globalX, int globalY, int width, int height, int currentCenterX, int currentCenterY) const {
    const QSize windowSize(qMax(1, width), qMax(1, height));

    QScreen* screen = QGuiApplication::screenAt(QPoint(currentCenterX, currentCenterY));
    if (!screen) {
        const QPoint candidateCenter(globalX + windowSize.width() / 2,
                                     globalY + windowSize.height() / 2);
        screen = QGuiApplication::screenAt(candidateCenter);
    }
    if (!screen)
        screen = QGuiApplication::primaryScreen();

    return clamp_to_screen(screen, globalX, globalY, windowSize);
}
//----------------------------------------
void KtAlarmClockController::clock_timeout_impl(int state) {
    if (debugLocate_) qDebug() << "[Controller] clock_timeout_impl state=" << state;

    switch (state) {
    case KtAlarmClock::WorkTime:
        enter_phase(KtAlarmClock::WorkBreak);
        break;
    case KtAlarmClock::WorkBreak:
        enter_phase(KtAlarmClock::WorkTime, true);
        break;
    default:
        enter_phase(KtAlarmClock::None);
        break;
    }
}
//----------------------------------------
void KtAlarmClockController::close_all_windows() {
    if (debugLocate_) qDebug() << "[Controller] close_all_windows";

    set_phase(KtAlarmClock::None);
    loop_ = false;
    if (runtime_) runtime_->pause();
    force_unload_over_dlg();
    if (tray_) tray_->hide();

    if (cmd_) cmd_->forceQuit();
}
//----------------------------------------
void KtAlarmClockController::closeSettings() {
    if (model_)
        model_->settingsVisible(false);
}
//----------------------------------------
void KtAlarmClockController::dispatch_user_action(int actionId) {
    dispatch_user_action_impl(actionId);
}
//----------------------------------------
void KtAlarmClockController::dispatch_user_action_impl(int actionId) {
    if (debugLocate_) qDebug() << "[Controller] dispatch_user_action" << actionId;

    if (is_forbidden()) {
        if (debugLocate_) qDebug() << "[Controller] dispatch ignored (WorkBreak)";
        return;
    }

    run_command(actionId);
}
//----------------------------------------
void KtAlarmClockController::enter_break_phase() {
    if (runtime_) runtime_->pause();
    if (model_) model_->settingsVisible(false);
    load_over_dlg();
}
//----------------------------------------
void KtAlarmClockController::enter_idle_phase() {
    force_unload_over_dlg();
    loop_ = false;
    if (runtime_) runtime_->pause();
}
//----------------------------------------
void KtAlarmClockController::enter_phase(int phase, bool resetDurationFromParam) {
    if (debugLocate_)
        qDebug() << "[Controller] enter_phase" << phase << "resetFromParam=" << resetDurationFromParam;

    set_phase(phase);

    switch (phase) {
    case KtAlarmClock::WorkBreak:
        enter_break_phase();
        break;
    case KtAlarmClock::WorkTime:
        enter_work_phase(resetDurationFromParam);
        break;
    default:
        enter_idle_phase();
        break;
    }

    sync_model();
}
//----------------------------------------
void KtAlarmClockController::enter_work_phase(bool resetDurationFromParam) {
    force_unload_over_dlg();

    int counter = runtime_ ? runtime_->counter() : 0;
    if (resetDurationFromParam || counter <= 0)
        counter = parameter->WorkTime;
    if (runtime_) runtime_->start(KtAlarmClock::WorkTime, counter);
}
//----------------------------------------
void KtAlarmClockController::force_unload_over_dlg() {
    if (!lockScreen_ || !lockScreen_->get_visible())
        return;

    if (debugLocate_) qDebug() << "[Controller] force_unload_over_dlg";
    lockScreen_->set_exiting(true);
    lockScreen_->hide();
    sync_model();
}
//----------------------------------------
void KtAlarmClockController::handle_play_pause_action() {
    if (!runtime_)
        return;

    if (runtime_->counter() <= kInitialCounter) {
        loop_ = true;
        enter_phase(KtAlarmClock::WorkTime, true);
    }
    else if (runtime_->running() || phase_ == KtAlarmClock::WorkTime) {
        loop_ = false;
        set_phase(KtAlarmClock::None);
        runtime_->pause();
    }
    else {
        loop_ = true;
        enter_phase(KtAlarmClock::WorkTime);
    }

    if (debugLocate_)
        qDebug() << "[Controller] PlayPause loop=" << loop_
                 << "running=" << runtime_->running()
                 << "counter=" << runtime_->counter() << "phase=" << phase_;
}
//----------------------------------------
void KtAlarmClockController::handle_time_adjustment(int deltaSeconds) {
    if (!runtime_)
        return;

    loop_ = true;
    const int counter = qMax(0, runtime_->counter() + deltaSeconds);
    runtime_->start(KtAlarmClock::WorkTime, counter);
    set_phase(KtAlarmClock::WorkTime);
    force_unload_over_dlg();
}
//----------------------------------------
QVariantMap KtAlarmClockController::initialWindowPosition(int width, int height) const {
    const QSize windowSize(qMax(1, width), qMax(1, height));

    QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
    if (!screen)
        screen = QGuiApplication::primaryScreen();
    if (!screen)
        return {{QStringLiteral("x"), 600}, {QStringLiteral("y"), 2}};

    const QRect available = screen->availableGeometry();
    const int x = available.left() + qMin(600, qMax(0, available.width() - windowSize.width()));
    const int y = available.top() + 2;
    return clamp_to_screen(screen, x, y, windowSize);
}
//----------------------------------------
bool KtAlarmClockController::is_forbidden() const {
    return phase_ == KtAlarmClock::WorkBreak;
}
//----------------------------------------
void KtAlarmClockController::load_over_dlg() {
    if (!lockScreen_)
        return;

    if (debugLocate_) qDebug() << "[Controller] load_over_dlg";
    lockScreen_->set_exiting(false);
    lockScreen_->show(parameter->WorkBreak, parameter->TimeForce, lockDebugMode_);
    sync_model();
}
//----------------------------------------
void KtAlarmClockController::nextLoop() {
    dispatch_user_action(KtAlarmClock::ActionNextLoop);
}
//----------------------------------------
void KtAlarmClockController::on_clock_timeout(int state) {
    if (debugLocate_) qDebug() << "[Controller] clock_timeout defer state=" << state;
    QTimer::singleShot(0, this, [this, state]() { clock_timeout_impl(state); });
}
//----------------------------------------
void KtAlarmClockController::on_clock_updated(int state, int counterSec, bool running) {
    Q_UNUSED(state);
    if (!model_)
        return;

    model_->set_running_state(running);
    model_->set_remaining_seconds(counterSec);
    model_->set_phase_state(phase_);
    model_->loop(loop_);

    if (tray_) {
        tray_->setRunning(running);
        tray_->setRemainingText(model_->remainingText());
    }
}
//----------------------------------------
void KtAlarmClockController::openSettings() {
    if (is_forbidden())
        return;

    if (model_) {
        model_->sync_from_param(*parameter);
        model_->settingsVisible(true);
    }
}
//----------------------------------------
void KtAlarmClockController::playPause() {
    dispatch_user_action(KtAlarmClock::ActionPlayPause);
}
//----------------------------------------
void KtAlarmClockController::quit() {
    dispatch_user_action(KtAlarmClock::ActionClose);
}
//----------------------------------------
void KtAlarmClockController::resetSettings() {
    parameter->setDefault();
    parameter->save();
    sync_model();
}
//----------------------------------------
void KtAlarmClockController::run_command(int actionId) {
    if (debugLocate_) qDebug() << "[Controller] run_command" << actionId;

    switch (actionId) {
    case KtAlarmClock::ActionPlayPause:
        handle_play_pause_action();
        break;
    case KtAlarmClock::ActionBreak:
        loop_ = true;
        enter_phase(KtAlarmClock::WorkBreak);
        break;
    case KtAlarmClock::ActionNextLoop:
        loop_ = true;
        enter_phase(KtAlarmClock::WorkTime, true);
        break;
    case KtAlarmClock::ActionForward:
        handle_time_adjustment(-kMinuteStepSeconds);
        break;
    case KtAlarmClock::ActionBackward:
        handle_time_adjustment(kMinuteStepSeconds);
        break;
    case KtAlarmClock::ActionMainDlg:
        openSettings();
        break;
    case KtAlarmClock::ActionClose:
        close_all_windows();
        break;
    case KtAlarmClock::ActionHelp:
        QDesktopServices::openUrl(
            QUrl(QStringLiteral("https://gitee.com/PhoenixWing321/KtAlarmClock/wikis/Home")));
        break;
    case KtAlarmClock::ActionKtWeb:
        QDesktopServices::openUrl(
            QUrl(QStringLiteral("https://gitee.com/PhoenixWing321/KtAlarmClock")));
        break;
    default:
        break;
    }

    sync_model();
}
//----------------------------------------
void KtAlarmClockController::saveSettings(int workSeconds, int breakSeconds, int forceSeconds) {
    parameter->WorkTime  = workSeconds;
    parameter->WorkBreak = breakSeconds;
    parameter->TimeForce = forceSeconds;
    parameter->save();

    if (model_) model_->settingsVisible(false);
    sync_model();
}
//----------------------------------------
void KtAlarmClockController::saveSettingsAndStart(int workSeconds, int breakSeconds,
                                                  int forceSeconds) {
    parameter->WorkTime  = workSeconds;
    parameter->WorkBreak = breakSeconds;
    parameter->TimeForce = forceSeconds;
    parameter->save();

    if (model_) model_->settingsVisible(false);

    loop_ = true;
    enter_phase(KtAlarmClock::WorkTime, true);
    sync_model();
}
//----------------------------------------
void KtAlarmClockController::set_debug_locate(bool enabled) {
    debugLocate_ = enabled;
    if (tray_) tray_->setDebugLocate(enabled);
}
//----------------------------------------
void KtAlarmClockController::set_phase(int phase) {
    phase_ = phase;
    if (model_) model_->set_phase_state(phase_);
    if (tray_) tray_->setForbidden(phase_ == KtAlarmClock::WorkBreak);
}
//----------------------------------------
void KtAlarmClockController::setLocale(const QString& localeName) {
    const QString normalizedLocale = KtAlarmClockSettings::normalize_locale(localeName);
    if (model_ && model_->locale() == normalizedLocale)
        return;

    KtAlarmClockSettings::write_locale(normalizedLocale);

    if (translationManager_) {
        const bool loaded = translationManager_->install(normalizedLocale);
        if (!loaded)
            qWarning() << "[Controller] failed to load locale" << normalizedLocale;
    }

    if (model_)
        model_->locale(normalizedLocale);
    if (qmlShell_)
        qmlShell_->retranslate();
    if (lockScreen_)
        lockScreen_->retranslate();
    if (tray_)
        tray_->retranslate();
}
//----------------------------------------
void KtAlarmClockController::showContextMenu(int globalX, int globalY) {
    if (tray_)
        tray_->showContextMenuAt(QPoint(globalX, globalY));
}
//----------------------------------------
void KtAlarmClockController::start() {
    runtime_ = new KtClockRuntime(this);
    lockScreen_ = new KtLockScreenManager(this);
    tray_ = new KtTrayService(this);
    tray_->setDebugLocate(debugLocate_);

    connect(runtime_, &KtClockRuntime::timeout, this, &KtAlarmClockController::on_clock_timeout);
    connect(runtime_, &KtClockRuntime::updated, this, &KtAlarmClockController::on_clock_updated);
    connect(lockScreen_, &KtLockScreenManager::clock_out, this,
            &KtAlarmClockController::on_clock_timeout);
    connect(lockScreen_, &KtLockScreenManager::visible_changed, this,
            &KtAlarmClockController::sync_model);

    connect(tray_, &KtTrayService::playPauseRequested, this, &KtAlarmClockController::playPause);
    connect(tray_, &KtTrayService::breakRequested, this, &KtAlarmClockController::startBreak);
    connect(tray_, &KtTrayService::nextLoopRequested, this, &KtAlarmClockController::nextLoop);
    connect(tray_, &KtTrayService::settingsRequested, this, &KtAlarmClockController::openSettings);
    connect(tray_, &KtTrayService::helpRequested, this, [this]() {
        dispatch_user_action(KtAlarmClock::ActionHelp);
    });
    connect(tray_, &KtTrayService::websiteRequested, this, [this]() {
        dispatch_user_action(KtAlarmClock::ActionKtWeb);
    });
    connect(tray_, &KtTrayService::quitRequested, this, &KtAlarmClockController::quit);

    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this,
            [this](Qt::ApplicationState state) {
                if (state == Qt::ApplicationActive && runtime_ && runtime_->running()) {
                    runtime_->sync_from_monotonic_clock();
                    sync_model();
                }
            });

    qmlShell_ = new KtAlarmClockQmlShell(model_, this, this);
    qmlShell_->start();
    sync_model();

    if (debugLocate_)
        qDebug() << "[Controller] start complete lockDebugMode=" << lockDebugMode_;
}
//----------------------------------------
void KtAlarmClockController::startBreak() {
    dispatch_user_action(KtAlarmClock::ActionBreak);
}
//----------------------------------------
void KtAlarmClockController::sync_model() {
    if (!model_)
        return;

    model_->sync_from_param(*parameter);
    model_->set_phase_state(phase_);
    model_->loop(loop_);
    model_->lockVisible(lockScreen_ && lockScreen_->get_visible());

    if (runtime_) {
        model_->set_running_state(runtime_->running());
        model_->set_remaining_seconds(runtime_->counter());
    }

    if (tray_) {
        tray_->setForbidden(phase_ == KtAlarmClock::WorkBreak);
        tray_->setRunning(model_->running());
        tray_->setRemainingText(model_->remainingText());
    }
}
