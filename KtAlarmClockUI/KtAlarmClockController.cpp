/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     Apache-2.0
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
#include "KtScreenUtil.h"

#include <QCursor>
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
    , debugLocate_(false) {         // 10；Debug 构建在 Cmd::build 中开启
}
//------------------------------------------------------
KtAlarmClockController::~KtAlarmClockController() {
    close_setting_dlg();
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
        close_setting_dlg();
        mainClock_->hide();
        load_over_dlg();
        return;
    case KtAlarmClock::WorkTime: {
        mainClock_->show();
        force_unload_over_dlg();
        int counter = mainClock_->get_counter();
        if (resetDurationFromParam || counter <= 0)
            counter = parameter->WorkTime; // Next / 到期 / 首次：以界面参数为准
        mainClock_->clock_start(KtAlarmClock::WorkTime, counter, !resetDurationFromParam);
        break;
    }
    default:
        mainClock_->show();
        force_unload_over_dlg();
        loop_ = false;
        mainClock_->clock_pause();
        break;
    }

    refresh_setting_ui();
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
    close_setting_dlg();

    if (cmd_) cmd_->forceQuit();
}
//------------------------------------------------------
void KtAlarmClockController::close_setting_dlg() {
    if (settingDlg_.isNull()) return;
    if (debugLocate_) qDebug() << "[Setting] close";

    settingDlg_->hide();
    delete settingDlg_.data(); // settingDlg_ 由 QPointer 自动置空
}
//------------------------------------------------------
void KtAlarmClockController::dispatch_user_action(int actionId) {
    if (debugLocate_) qDebug() << "[Controller] dispatch_user_action" << actionId;

    if (is_forbidden()) {
        if (debugLocate_) qDebug() << "[Controller] dispatch ignored (WorkBreak)";
        return;
    }

    run_command(actionId);
}
//------------------------------------------------------
void KtAlarmClockController::force_unload_over_dlg() {
    if (!lockScreen_->get_visible()) return;
    if (debugLocate_) qDebug() << "[Controller] force_unload_over_dlg";
    lockScreen_->set_exiting(true);
    lockScreen_->dismiss();
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
void KtAlarmClockController::on_second_instance_activate() {
    if (is_forbidden()) {
        if (tray_) {
            tray_->show_message(QStringLiteral("KT护眼闹钟"),
                                QStringLiteral("正在休息中，请从托盘或等待休息结束。"));
        }
        return;
    }

    if (!mainClock_)
        return;

    mainClock_->show();
    mainClock_->raise();
    mainClock_->activateWindow();
}
//------------------------------------------------------
void KtAlarmClockController::on_setting_action(int actionId) {
    sync_param_from_setting();
    dispatch_user_action(actionId);
}
//------------------------------------------------------
void KtAlarmClockController::refresh_setting_ui() {
    if (settingDlg_.isNull()) return;

    settingDlg_->update_dialog();
    if (mainClock_)
        settingDlg_->update_play_pause_button(mainClock_->get_running());
}
//------------------------------------------------------
void KtAlarmClockController::run_command(int actionId) {
    if (debugLocate_) qDebug() << "[Controller] run_command" << actionId;

    // 托盘/右键等同路径：设置窗仍打开时写回未提交的编辑
    switch (actionId) {
    case KtAlarmClock::ActionPlayPause:
    case KtAlarmClock::ActionBreak:
    case KtAlarmClock::ActionNextLoop:
        sync_param_from_setting();
        break;
    default:
        break;
    }

    switch (actionId) {
    case KtAlarmClock::ActionPlayPause:
        mainClock_->show();
        if (mainClock_->get_counter() <= kInitialCounter) {
            loop_ = true;
            clock_start(KtAlarmClock::WorkTime, true); // 首次播放：以 parameter 为准
        }
        else if (mainClock_->get_running()) {
            loop_     = false;
            workStep_ = KtAlarmClock::None;
            mainClock_->clock_pause();
        }
        else {
            loop_ = true;
            clock_start(KtAlarmClock::WorkTime); // 手动暂停或休眠冻结后续计
        }
        if (debugLocate_)
            qDebug() << "[Controller] PlayPause loop=" << loop_
                     << "running=" << mainClock_->get_running()
                     << "counter=" << mainClock_->get_counter() << "workStep=" << workStep_;
        break;
    case KtAlarmClock::ActionBreak:
        loop_ = true;
        clock_start(KtAlarmClock::WorkBreak);
        break;
    case KtAlarmClock::ActionNextLoop:
        loop_ = true;
        clock_start(KtAlarmClock::WorkTime, true); // Next：以 parameter 重计 WorkTime
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
        show_setting_dlg(QCursor::pos());
        break;
    case KtAlarmClock::ActionClose:
        close_all_windows();
        break;
    case KtAlarmClock::ActionHelp:
        QDesktopServices::openUrl(
            QUrl(QStringLiteral("https://gitee.com/phoenixwing/KtAlarmClock/wikis/Home")));
        break;
    case KtAlarmClock::ActionKtWeb:
        QDesktopServices::openUrl(
            QUrl(QStringLiteral("https://gitee.com/phoenixwing/KtAlarmClock")));
        break;
    default:
        break;
    }

    refresh_setting_ui();
}
//------------------------------------------------------
void KtAlarmClockController::set_debug_locate(bool enabled) {
    debugLocate_ = enabled;
    if (tray_) tray_->set_debug_locate(enabled);
    if (!settingDlg_.isNull()) settingDlg_->set_debug_locate(enabled);
}
//------------------------------------------------------
void KtAlarmClockController::show_pop_menu(const QPoint& globalPos) {
    if (!popMenu_) return;
    if (debugLocate_)
        qDebug() << "[PopMenu] popup at" << globalPos << "forbidden=" << is_forbidden();
    if (is_forbidden()) return;

    popMenu_->ensurePolished();
    const QPoint menuPos =
        KtScreenUtil::place_popup(globalPos, popMenu_->sizeHint(), true);
    popMenu_->popup(menuPos);
}
//------------------------------------------------------
void KtAlarmClockController::show_setting_dlg(const QPoint& globalPos) {
    if (settingDlg_.isNull()) {
        settingDlg_ = new KtAlarmClockSettingWindow();
        settingDlg_->set_debug_locate(debugLocate_);
        connect(settingDlg_, &KtAlarmClockSettingWindow::action_triggered, this,
                &KtAlarmClockController::on_setting_action);
        if (debugLocate_) qDebug() << "[Setting] create at" << globalPos;
    }
    else if (debugLocate_) {
        qDebug() << "[Setting] reuse at" << globalPos;
    }

    settingDlg_->set_param(parameter);
    refresh_setting_ui();
    settingDlg_->show_near(globalPos);
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
            dispatch_user_action(actionId);
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
    connect(tray_, &KtAlarmClockTray::action_triggered, this,
            &KtAlarmClockController::dispatch_user_action);

    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this,
            [ this ](Qt::ApplicationState state) {
                if (!mainClock_) return;
                if (state == Qt::ApplicationSuspended)
                    mainClock_->handle_application_suspended();
                else if (state == Qt::ApplicationActive)
                    mainClock_->handle_application_active();
            });

    if (debugLocate_) qDebug() << "[Controller] start complete";
}
//------------------------------------------------------
void KtAlarmClockController::sync_param_from_setting() {
    if (settingDlg_.isNull()) return;
    settingDlg_->update_infos(); // 以 m:ss 输入框秒数写回 parameter
    parameter->registerWrite();  // 持久化到注册表
    if (debugLocate_)
        qDebug() << "[Setting] sync_param_from_setting saved Work=" << parameter->WorkTime
                 << "Break=" << parameter->WorkBreak << "Force=" << parameter->TimeForce;
}
