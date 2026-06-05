/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockController.cpp
 */
#include "KtAlarmClockController.h"

#include "KtAlarmClock.h"
#include "KtAlarmClockCmd.h"
#include "KtAlarmClockParam.h"
#include "KtAlarmClockSettingWindow.h"
#include "KtAlarmClockTray.h"
#include "KtLockScreenManager.h"
#include "KtMainClockWidget.h"

#include <QDebug>
#include <QDesktopServices>
#include <QFontMetrics>
#include <QGuiApplication>
#include <QMenu>
#include <QTimer>
#include <QUrl>

//------------------------------------------------------
KtAlarmClockController::KtAlarmClockController(KtAlarmClockParamShared param, KtAlarmClockCmd* cmd,
                                               QObject* parent)
    : QObject(parent)
    , parameter(param)              // 1
    , cmd_(cmd)                     // 2
    , mainClock_(nullptr)           // 3
    , tray_(nullptr)                // 4
    , lockScreen_(nullptr)          // 5
    , settingDlg_(nullptr)          // 6
    , popMenu_(nullptr)             // 7
    , loop_(false)                  // 8
    , workStep_(KtAlarmClock::None) // 9
    , debugLocate_(true) {          // 10
}
//------------------------------------------------------
KtAlarmClockController::~KtAlarmClockController() {
    unload_setting_dlg();
    delete popMenu_;
    popMenu_ = nullptr; // 7
    delete mainClock_;
    mainClock_ = nullptr; // 3
    delete tray_;
    tray_ = nullptr; // 4
    delete lockScreen_;
    lockScreen_ = nullptr; // 5
    // parameter                 // 1 外部拥有
    // cmd_                   // 2 外部拥有
    // settingDlg_            // 6 unload 已处理
    // loop_                  // 8
    // workStep_              // 9
    // debugLocate_           // 10
}
//------------------------------------------------------
void KtAlarmClockController::apply_debug_defaults() {
    // 默认关闭
    Q_UNUSED(parameter);
}
//------------------------------------------------------
void KtAlarmClockController::clock_start(int workStep, bool resetDurationFromParam) {
    if (debugLocate_)
        qDebug() << "[Controller] clock_start" << workStep << "resetFromParam=" << resetDurationFromParam;

    workStep_ = workStep;
    if (tray_) tray_->set_forbidden(workStep_ == KtAlarmClock::WorkBreak);

    switch (workStep) {
    case KtAlarmClock::WorkBreak:
        mainClock_->clock_pause();
        unload_setting_dlg();
        mainClock_->hide();
        load_over_dlg();
        return;
    case KtAlarmClock::WorkTime: {
        mainClock_->show();
        force_unload_over_dlg();
        int counter = mainClock_->get_counter();
        if (resetDurationFromParam || counter <= 0)
            counter = parameter->WorkTime; // Next / 到期 / 首次：以界面参数为准
        mainClock_->clock_start(KtAlarmClock::WorkTime, counter);
        break;
    }
    default:
        mainClock_->show();
        force_unload_over_dlg();
        loop_ = false;
        mainClock_->clock_pause();
        break;
    }
}
//------------------------------------------------------
void KtAlarmClockController::clock_timeout_impl(int state) {
    if (debugLocate_) qDebug() << "[Controller] clock_timeout_impl state=" << state;

    switch (state) {
    case KtAlarmClock::WorkTime:
        sync_param_from_setting();
        clock_start(KtAlarmClock::WorkBreak);
        break;
    case KtAlarmClock::WorkBreak:
        sync_param_from_setting();
        clock_start(KtAlarmClock::WorkTime, true); // 休息结束：重置为 WorkTime 参数
        break;
    default:
        clock_start(KtAlarmClock::None);
        break;
    }
}
//------------------------------------------------------
void KtAlarmClockController::close_all_windows() {
    if (debugLocate_) qDebug() << "[Controller] close_all_windows";

    workStep_ = KtAlarmClock::None;
    loop_     = false;
    mainClock_->set_can_close(true);
    mainClock_->clock_pause();
    force_unload_over_dlg();
    if (tray_) tray_->hide();
    unload_setting_dlg();

    if (cmd_) cmd_->forceQuit();
}
//------------------------------------------------------
void KtAlarmClockController::force_unload_over_dlg() {
    if (!lockScreen_->get_visible()) return;
    if (debugLocate_) qDebug() << "[Controller] force_unload_over_dlg";
    lockScreen_->set_exiting(true);
    lockScreen_->hide();
}
//------------------------------------------------------
bool KtAlarmClockController::is_forbidden() const {
    return workStep_ == KtAlarmClock::WorkBreak;
}
//------------------------------------------------------
void KtAlarmClockController::load_over_dlg() {
    if (debugLocate_) qDebug() << "[Controller] load_over_dlg";
    lockScreen_->set_exiting(false);
    // debugLocate_ 仅控制日志；锁屏多屏遮罩须 debugLayout_=false 才铺满各显示器
    lockScreen_->show(parameter->WorkBreak, parameter->TimeForce, false);
}
//------------------------------------------------------
void KtAlarmClockController::load_setting_dlg() {
    unload_setting_dlg();
    settingDlg_ = new KtAlarmClockSettingWindow();
    settingDlg_->set_debug_locate(debugLocate_);

    // 设置参数并更新对话框
    settingDlg_->set_param(parameter);
    settingDlg_->update_dialog();

    // 动作连接
    connect(settingDlg_, &KtAlarmClockSettingWindow::action_triggered, this,
            &KtAlarmClockController::run_command);

    // 销毁连接
    connect(settingDlg_, &QWidget::destroyed, this, [ this ]() { settingDlg_ = nullptr; });

    // 显示对话框
    settingDlg_->show_near(mainClock_);

    if (debugLocate_) qDebug() << "[Setting] load_setting_dlg";
}
//------------------------------------------------------
void KtAlarmClockController::on_clock_timeout(int state) {
    if (debugLocate_) qDebug() << "[Controller] clock_timeout defer state=" << state;
    // 延迟切换，避免在锁屏信号栈内销毁 UI
    QTimer::singleShot(0, this, [ this, state ]() { clock_timeout_impl(state); });
}
//------------------------------------------------------
void KtAlarmClockController::on_context_menu_requested(const QPoint& globalPos) {
    show_pop_menu(globalPos);
}
//------------------------------------------------------
void KtAlarmClockController::run_command(int actionId) {
    if (debugLocate_) qDebug() << "[Controller] run_command" << actionId;

    switch (actionId) {
    case KtAlarmClock::ActionPlayPause:
        mainClock_->show();
        if (mainClock_->get_counter() <= kInitialCounter) {
            loop_ = true;
            sync_param_from_setting();
            clock_start(KtAlarmClock::WorkTime, true); // 首次播放：以界面参数为准
        }
        else if (mainClock_->get_running() || workStep_ == KtAlarmClock::WorkTime) {
            loop_     = false;
            workStep_ = KtAlarmClock::None;
            mainClock_->clock_pause();
        }
        else {
            loop_ = true;
            if (mainClock_->get_counter() <= 0) sync_param_from_setting();
            clock_start(KtAlarmClock::WorkTime);
        }
        if (debugLocate_)
            qDebug() << "[Controller] PlayPause loop=" << loop_
                     << "running=" << mainClock_->get_running()
                     << "counter=" << mainClock_->get_counter() << "workStep=" << workStep_;
        break;
    case KtAlarmClock::ActionBreak:
        sync_param_from_setting();
        loop_ = true;
        clock_start(KtAlarmClock::WorkBreak);
        break;
    case KtAlarmClock::ActionNextLoop:
        loop_ = true;
        sync_param_from_setting();
        clock_start(KtAlarmClock::WorkTime, true); // Next：写回参数并重计 WorkTime
        break;
    case KtAlarmClock::ActionForward:
        loop_ = true;
        mainClock_->clock_start(KtAlarmClock::WorkTime, qMax(0, mainClock_->get_counter() - 60));
        workStep_ = KtAlarmClock::WorkTime;
        mainClock_->show();
        force_unload_over_dlg();
        break;
    case KtAlarmClock::ActionBackward:
        loop_ = true;
        mainClock_->clock_start(KtAlarmClock::WorkTime, mainClock_->get_counter() + 60);
        workStep_ = KtAlarmClock::WorkTime;
        mainClock_->show();
        force_unload_over_dlg();
        break;
    case KtAlarmClock::ActionMainDlg:
        load_setting_dlg();
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
}
//------------------------------------------------------
void KtAlarmClockController::set_debug_locate(bool enabled) {
    debugLocate_ = enabled;
    if (tray_) tray_->set_debug_locate(enabled);
    if (settingDlg_) settingDlg_->set_debug_locate(enabled);
}
//------------------------------------------------------
void KtAlarmClockController::show_pop_menu(const QPoint& globalPos) {
    if (!popMenu_) return;
    if (debugLocate_)
        qDebug() << "[PopMenu] popup at" << globalPos << "forbidden=" << is_forbidden();
    if (is_forbidden()) return;
    popMenu_->popup(globalPos);
}
//------------------------------------------------------
void KtAlarmClockController::start() {
    apply_debug_defaults();

    mainClock_ = new KtMainClockWidget();
    mainClock_->clock_pause();
    mainClock_->show();

    lockScreen_ = new KtLockScreenManager(this);

    tray_ = new KtAlarmClockTray(this);
    tray_->set_debug_locate(debugLocate_);

    // 勿以 mainClock_ 为父：胶囊窗太窄会导致 QMenu 宽度被裁切
    popMenu_ = new QMenu();
    auto addPop = [ this ](const QString& text, int actionId) {
        QAction* action = popMenu_->addAction(text);
        connect(action, &QAction::triggered, this, [ this, actionId ]() {
            if (is_forbidden()) return;
            run_command(actionId);
        });
    };
    addPop(QStringLiteral("立刻休息"), KtAlarmClock::ActionBreak);
    addPop(QStringLiteral("显示设置界面"), KtAlarmClock::ActionMainDlg);
    addPop(QStringLiteral("在线帮助"), KtAlarmClock::ActionHelp);
    addPop(QStringLiteral("开源代码主页"), KtAlarmClock::ActionKtWeb);

    int maxTextWidth = 0;
    const QFontMetrics fontMetrics(popMenu_->font());
    for (QAction* action : popMenu_->actions())
        maxTextWidth = qMax(maxTextWidth, fontMetrics.horizontalAdvance(action->text()));
    const int menuMinWidth = maxTextWidth + 48; // 左右内边距 + 勾选预留
    popMenu_->setStyleSheet(
        QStringLiteral("QMenu { min-width: %1px; } QMenu::item { padding: 4px 28px 4px 12px; }")
            .arg(menuMinWidth));

    connect(mainClock_, &KtMainClockWidget::clock_out, this,
            &KtAlarmClockController::on_clock_timeout);
    connect(lockScreen_, &KtLockScreenManager::clock_out, this,
            &KtAlarmClockController::on_clock_timeout);
    connect(mainClock_, &KtMainClockWidget::context_menu_requested, this,
            &KtAlarmClockController::on_context_menu_requested);
    connect(tray_, &KtAlarmClockTray::action_triggered, this, &KtAlarmClockController::run_command);

    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this,
            [ this ](Qt::ApplicationState state) {
                if (state == Qt::ApplicationActive && mainClock_->get_running())
                    mainClock_->sync_from_wall_clock(); // 休眠唤醒校准
            });

    if (debugLocate_) qDebug() << "[Controller] start complete";
}
//------------------------------------------------------
void KtAlarmClockController::sync_param_from_setting() {
    if (!settingDlg_)
        return;
    settingDlg_->update_infos(); // 以 spinBox 秒数写回 parameter
    parameter->registerWrite();  // 持久化到注册表
    if (debugLocate_)
        qDebug() << "[Setting] sync_param_from_setting saved Work=" << parameter->WorkTime
                 << "Break=" << parameter->WorkBreak << "Force=" << parameter->TimeForce;
}
//------------------------------------------------------
void KtAlarmClockController::unload_setting_dlg() {
    if (!settingDlg_) return;
    if (debugLocate_) qDebug() << "[Setting] unload_setting_dlg";
    settingDlg_->close();
    settingDlg_->deleteLater();
    settingDlg_ = nullptr;
}
