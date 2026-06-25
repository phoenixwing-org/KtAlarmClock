/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtLockScreenManager.cpp
 */
#include "KtLockScreenManager.h"

#include "KtAlarmClock.h"
#include "KtLockScreenPrimaryWidget.h"
#include "KtLockScreenSecondaryWidget.h"
#include "KtWindowsWakeNotifier.h"

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QGuiApplication>
#include <QRandomGenerator>
#include <QScreen>
#include <QTimer>
#include <QtMath>

namespace {
// 与主浮窗一致：短于该间隔视为 UI 卡顿，长于该间隔视为真休眠/关屏
constexpr int kSleepGapMs = 12000;
} // namespace

/** @brief 将秒数格式化为 m:ss */
static QString format_clock(int counter) {
    if (counter <= 0) return QStringLiteral("0:00"); // 到期或无效统一显示 0:00
    const int minutePart = counter / 60;
    const int secondPart = counter % 60;
    return QStringLiteral("%1:%2").arg(minutePart).arg(secondPart, 2, 10, QChar('0'));
}

/** @brief 从 qrc 加载 lock-screen.qss */
static QString load_lock_screen_style_sheet() {
    QFile qssFile(QStringLiteral(":/lock-screen.qss"));
    if (!qssFile.open(QIODevice::ReadOnly)) return QString(); // 无 qss 时走控件默认样式
    return QString::fromUtf8(qssFile.readAll());
}
//------------------------------------------------------
KtLockScreenManager::KtLockScreenManager(QObject* parent)
    : QObject(parent)
    , breakClock_()             // 1
    , primary_(nullptr)         // 2
    , tickTimer_(nullptr)       // 3
    , watchdogTimer_(nullptr)   // 4
    , wakeWatchTimer_(nullptr)  // 5
    , visible_(false)           // 6
    , exiting_(false)           // 7
    , debugMode_(false)         // 8
    , unlockPending_(false)     // 9
    , isForced_(false)          // 10
    , counterForce_(0)          // 11
    , forceEndMs_(0)            // 12
    , formulaValue_(0)          // 13
    , primaryScreenId_(0)       // 14
    , secondaries_()            // 15
    , debugSizeFraction_(1.0)   // 16
    , lastTickMs_(0)            // 17
    , sleepStartedMs_(0) {      // 18
    // 1s 同步休息倒计时；10s 检查显示器热插拔

    tickTimer_ = new QTimer(this);
    tickTimer_->setInterval(1000); // 1s
    connect(tickTimer_, &QTimer::timeout, this, &KtLockScreenManager::on_tick);

    watchdogTimer_ = new QTimer(this);
    watchdogTimer_->setInterval(10000); // 10s
    connect(watchdogTimer_, &QTimer::timeout, this, &KtLockScreenManager::on_watchdog);

    wakeWatchTimer_ = new QTimer(this);
    wakeWatchTimer_->setInterval(1000);
    connect(wakeWatchTimer_, &QTimer::timeout, this, &KtLockScreenManager::on_wake_watchdog);
    wakeWatchTimer_->start();

    auto* wakeNotifier = new KtWindowsWakeNotifier(this);
    connect(wakeNotifier, &KtWindowsWakeNotifier::system_resumed, this,
            &KtLockScreenManager::try_resume_after_wake);

    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this,
            &KtLockScreenManager::on_application_state_changed); // 休眠唤醒校正
    connect(qGuiApp, &QGuiApplication::screenAdded, this, [this]() {
        if (visible_) reconcile_screens(); // 新显示器：补副屏遮罩
    });
    connect(qGuiApp, &QGuiApplication::screenRemoved, this, [this](QScreen*) {
        if (visible_) reconcile_screens(); // 拔线：立刻隐藏并销毁多余遮罩
    });
}
//------------------------------------------------------
KtLockScreenManager::~KtLockScreenManager() {
    dismiss(); // 内部已 delete primary_

    // breakClock_            // 1
    // primary_               // 2 dismiss() 已销毁
    tickTimer_       = nullptr; // 3
    watchdogTimer_   = nullptr; // 4
    wakeWatchTimer_  = nullptr; // 5
    // visible_                 // 6
    // exiting_               // 6
    // debugMode_             // 7
    // unlockPending_         // 8
    // isForced_              // 9
    // counterForce_          // 10
    // forceEndMs_            // 11
    // formulaValue_          // 12
    // primaryScreenId_       // 13
    secondaries_.clear(); // 14
    // debugSizeFraction_     // 16
    // lastTickMs_            // 17
    // sleepStartedMs_        // 18
}
//------------------------------------------------------
void KtLockScreenManager::apply_primary_geometry() {
    if (!primary_widget()) return;

    primary_widget()->apply_screen_geometry(!debugMode_, debugMode_, debugSizeFraction_);
    if (debugMode_) primary_widget()->update_debug_size_button_text(debugSizeFraction_);

    for (auto* secondary : secondaries_)
        secondary->apply_screen_geometry(!debugMode_, debugMode_, primary_widget());
}
//------------------------------------------------------
void KtLockScreenManager::detect_sleep_gap() {
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (lastTickMs_ <= 0) {
        lastTickMs_ = now;
        return;
    }

    if (!breakClock_.get_running() || breakClock_.is_sleep_suspended()) {
        lastTickMs_ = now;
        return;
    }

    if (now - lastTickMs_ > kSleepGapMs)
        enter_system_sleep_at(lastTickMs_);

    lastTickMs_ = now;
}
//------------------------------------------------------
void KtLockScreenManager::enter_system_sleep() {
    enter_system_sleep_at(QDateTime::currentMSecsSinceEpoch());
}
//------------------------------------------------------
void KtLockScreenManager::enter_system_sleep_at(qint64 wallMs) {
    if (!visible_ || breakClock_.is_sleep_suspended())
        return;

    sleepStartedMs_ = wallMs;
    if (breakClock_.get_running())
        breakClock_.freeze_for_system_sleep_at(wallMs);

    if (tickTimer_)
        tickTimer_->stop();

    lastTickMs_ = wallMs;
    qDebug() << "[LockScreen] enter_system_sleep breakCounter=" << breakClock_.get_counter();
}
//------------------------------------------------------
QString KtLockScreenManager::format_break_time() const {
    return format_clock(breakClock_.get_counter());
}
//------------------------------------------------------
void KtLockScreenManager::dismiss() {
    // 停表 → 销毁遮罩窗口 → 重置状态

    tickTimer_->stop();
    watchdogTimer_->stop();

    // 副屏：直接销毁（show 时按屏数重建）
    for (auto* overlay : secondaries_) {
        overlay->hide();
        overlay->deleteLater();
    }
    secondaries_.clear();

    // 主屏：showFullScreen() 的窗口 hide() 后可能残留，直接销毁，show 时重建
    if (primary_) {
        primary_->set_debug_controls_visible(false);
        delete primary_.data();
        Q_ASSERT(primary_.isNull());
    }

    breakClock_.pause(); // 停止休息倒计时
    isForced_          = false;
    counterForce_      = 0;
    forceEndMs_        = 0;
    unlockPending_     = false;
    debugSizeFraction_ = 1.0;

    if (visible_) {
        visible_ = false;
        emit visible_changed(); // 通知 QML 显示主浮窗
    }
}
//------------------------------------------------------
void KtLockScreenManager::initial_formula() {
    // 随机加法题，答案存于 formulaValue_

    const int operandA = QRandomGenerator::global()->bounded(1000);
    const int operandB = QRandomGenerator::global()->bounded(1000);
    formulaValue_      = operandA + operandB;
    if (primary_widget())
        primary_widget()->set_formula_text(QStringLiteral("%1 + %2 =").arg(operandA).arg(operandB));
}
//------------------------------------------------------
void KtLockScreenManager::on_application_state_changed(Qt::ApplicationState state) {
    if (!visible_) return;

    switch (state) {
    case Qt::ApplicationSuspended:
    case Qt::ApplicationHidden:
        enter_system_sleep();
        break;
    case Qt::ApplicationActive:
        // 推迟到事件循环下一轮，避免在系统唤醒回调栈内重入 showFullScreen
        QTimer::singleShot(0, this, &KtLockScreenManager::try_resume_after_wake);
        break;
    default:
        break;
    }
}
//------------------------------------------------------
void KtLockScreenManager::on_debug_exit_requested() {
    if (!debugMode_ || !visible_) return;

    dismiss();
    emit clock_out(KtAlarmClock::WorkBreak); // 调试直接退出，恢复工作计时
}
//------------------------------------------------------
void KtLockScreenManager::on_debug_size_toggle_requested() {
    if (!debugMode_ || !primary_widget()) return;

    if (debugSizeFraction_ < 0.5)
        debugSizeFraction_ = 0.5; // 1/4 → 半屏
    else if (debugSizeFraction_ < 1.0)
        debugSizeFraction_ = 1.0; // 半屏 → 全屏
    else
        debugSizeFraction_ = 0.5; // 全屏 ↔ 半屏

    apply_primary_geometry();
    raise_all(true);
}
//------------------------------------------------------
void KtLockScreenManager::on_tick() {
    detect_sleep_gap();
    sync_wall_clocks(); // 每秒驱动休息/强制倒计时
}
//------------------------------------------------------
void KtLockScreenManager::on_unlock_requested() {
    try_unlock(false); // 主屏算式提交
}
//------------------------------------------------------
void KtLockScreenManager::on_wake_watchdog() {
    if (!visible_) return;

    const qint64 now = QDateTime::currentMSecsSinceEpoch();

    if (breakClock_.is_sleep_suspended()) {
        if (lastTickMs_ > 0 && now - lastTickMs_ >= kSleepGapMs)
            try_resume_after_wake();
        return;
    }

    if (breakClock_.get_running()) {
        breakClock_.realign_phase_start_if_ahead();
        if (tickTimer_ && !tickTimer_->isActive())
            tickTimer_->start();
    }
}
//------------------------------------------------------
void KtLockScreenManager::on_watchdog() {
    reconcile_screens(); // 热插拔 HDMI 等场景补副屏
}
//------------------------------------------------------
KtLockScreenPrimaryWidget* KtLockScreenManager::primary_widget() const {
    return primary_.data();
}
//------------------------------------------------------
void KtLockScreenManager::raise_all(bool requestFocus) {
    if (!visible_) return;

    if (primary_widget()) {
        if (requestFocus) {
            primary_widget()->raise();
            primary_widget()->activateWindow(); // 主屏需要输入算式
        }
        else {
            primary_widget()->raise_quiet();
        }
    }
    for (auto* secondary : secondaries_)
        secondary->raise_quiet(); // 副屏只置顶
}
//------------------------------------------------------
void KtLockScreenManager::reconcile_screens() {
    // 按当前屏幕数量维护遮罩：1 块 primary + 每块非主屏 1 块 secondary

    if (!visible_ || !primary_widget()) return;

    const auto screens = QGuiApplication::screens();
    if (screens.isEmpty()) return;

    QScreen* primaryScreen = QGuiApplication::primaryScreen();
    int      newPrimaryId  = primaryScreen ? screens.indexOf(primaryScreen) : 0;
    if (newPrimaryId < 0)
        newPrimaryId = 0;
    if (newPrimaryId >= screens.size())
        newPrimaryId = 0;

    if (primaryScreenId_ != newPrimaryId) {
        qDebug() << "[LockScreen] primary screen id" << primaryScreenId_ << "->" << newPrimaryId;
        primaryScreenId_ = newPrimaryId;
    }
    primary_widget()->set_screen_id(primaryScreenId_);

    QVector<int> neededScreenIds;
    if (debugMode_) {
        neededScreenIds.append(primaryScreenId_); // 调试只模拟一块副屏
    }
    else {
        for (int screenIndex = 0; screenIndex < screens.size(); ++screenIndex) {
            if (screenIndex != primaryScreenId_)
                neededScreenIds.append(screenIndex);
        }
    }

    qDebug() << "[LockScreen] reconcile_screens total=" << screens.size()
             << "primaryId=" << primaryScreenId_ << "needSecondary=" << neededScreenIds.size()
             << "haveSecondary=" << secondaries_.size();

    // 移除多余或索引失效的副屏遮罩（先 hide 再销毁，避免叠在同一块屏上）
    for (int index = secondaries_.size() - 1; index >= 0; --index) {
        auto* widget = secondaries_.at(index);
        if (!widget) {
            secondaries_.removeAt(index);
            continue;
        }

        const int screenId = widget->property("screenId").toInt();
        if (screenId < 0 || screenId >= screens.size() || !neededScreenIds.contains(screenId)) {
            qDebug() << "[LockScreen] remove secondary screenId=" << screenId;
            widget->hide();
            widget->deleteLater();
            secondaries_.removeAt(index);
        }
    }

    // 去重：同 screenId 只保留一块
    for (int index = secondaries_.size() - 1; index >= 0; --index) {
        const int screenId = secondaries_.at(index)->property("screenId").toInt();
        for (int prior = 0; prior < index; ++prior) {
            if (secondaries_.at(prior)->property("screenId").toInt() == screenId) {
                qDebug() << "[LockScreen] remove duplicate secondary screenId=" << screenId;
                secondaries_.at(index)->hide();
                secondaries_.at(index)->deleteLater();
                secondaries_.removeAt(index);
                break;
            }
        }
    }

    const QString overlayQss = load_lock_screen_style_sheet();

    // 缺少的副屏新建；已有的刷新几何
    for (int screenId : neededScreenIds) {
        KtLockScreenSecondaryWidget* existing = nullptr;
        for (auto* widget : secondaries_) {
            if (widget->property("screenId").toInt() == screenId) {
                existing = widget;
                break;
            }
        }

        if (existing) {
            existing->set_screen_id(screenId);
            existing->apply_screen_geometry(!debugMode_, debugMode_, primary_widget());
            existing->set_forced(isForced_, counterForce_);
            existing->show();
            existing->raise_quiet();
            continue;
        }

        auto* overlay = new KtLockScreenSecondaryWidget(screenId);
        overlay->setProperty("screenId", screenId);
        connect(overlay, &KtLockScreenSecondaryWidget::unlock_requested, this, [this]() {
            try_unlock(true);
        });
        overlay->apply_screen_geometry(!debugMode_, debugMode_, primary_widget());
        overlay->set_can_close(false);
        overlay->set_forced(isForced_, counterForce_);
        if (!overlayQss.isEmpty())
            overlay->setStyleSheet(overlayQss);

        secondaries_.append(overlay);
        overlay->show();
        qDebug() << "[LockScreen] created secondary screenId=" << screenId
                 << "geometry=" << overlay->geometry();
    }

    apply_primary_geometry(); // 主屏随当前 primaryScreen 重新铺满
    refresh_ui();

    qDebug() << "[LockScreen] reconcile done secondaryCount=" << secondaries_.size();
}
//------------------------------------------------------
void KtLockScreenManager::refresh_ui() {
    // 强制期隐藏解锁区；休息中显示算式与倒计时

    auto* primaryWidget = primary_widget();
    if (!primaryWidget) return;

    const bool showUnlock = !isForced_;
    primaryWidget->set_unlock_panel_visible(showUnlock);
    primaryWidget->set_formula_visible(showUnlock && breakClock_.get_running()); // 休息中才验算
    primaryWidget->set_break_time_text(breakClock_.get_running() ? format_break_time() : QString());
    primaryWidget->set_force_visible(false, 0); // 主屏仅保留休息总时长，不显示强制秒数

    for (auto* secondary : secondaries_)
        secondary->set_forced(isForced_, counterForce_); // 副屏仅控制解锁区，不显示第二计时
}
//------------------------------------------------------
void KtLockScreenManager::set_exiting(bool exiting) {
    if (exiting_ == exiting) return;
    exiting_ = exiting;
    emit exiting_changed();
}
//------------------------------------------------------
void KtLockScreenManager::show(int breakSeconds, int forceSeconds, bool debugMode) {
    // 显示锁屏：重置状态 → 启动休息计时 → 布局多屏 → 开定时器

    debugMode_ = debugMode;

    const auto screens       = QGuiApplication::screens();
    QScreen*   primaryScreen = QGuiApplication::primaryScreen();
    primaryScreenId_         = primaryScreen ? screens.indexOf(primaryScreen) : 0;
    if (primaryScreenId_ < 0) primaryScreenId_ = 0;

    qDebug() << "[LockScreen] show screens=" << screens.size()
             << "primaryScreenId=" << primaryScreenId_ << "debugMode=" << debugMode_;
    unlockPending_ = false;
    isForced_      = false;
    counterForce_  = 0;
    forceEndMs_    = 0;

    const QString lockQss = load_lock_screen_style_sheet();

    if (!primary_) primary_ = new KtLockScreenPrimaryWidget(primaryScreenId_); // 首次创建，后续复用
    connect(primary_, &KtLockScreenPrimaryWidget::answer_submitted, this,
            &KtLockScreenManager::on_unlock_requested, Qt::UniqueConnection);
    connect(primary_, &KtLockScreenPrimaryWidget::debug_exit_requested, this,
            &KtLockScreenManager::on_debug_exit_requested, Qt::UniqueConnection);
    connect(primary_, &KtLockScreenPrimaryWidget::debug_size_toggle_requested, this,
            &KtLockScreenManager::on_debug_size_toggle_requested, Qt::UniqueConnection);

    breakClock_.start(KtAlarmClock::WorkBreak, breakSeconds); // 启动休息墙钟

    // 强制等待期须短于休息总时长，否则全程可提前解锁
    if (forceSeconds > 0 && forceSeconds < breakSeconds) {
        counterForce_ = forceSeconds;
        forceEndMs_ = QDateTime::currentMSecsSinceEpoch() + forceSeconds * 1000LL; // 墙钟结束点
        isForced_ = true;
    }

    initial_formula();

    if (debugMode) {
        debugSizeFraction_ = 0.25; // 默认 1/4 屏，留桌面与托盘可操作
        primary_->set_can_close(true);
        primary_->set_debug_controls_visible(true);
        primary_->update_debug_size_button_text(debugSizeFraction_);
    }
    else {
        debugSizeFraction_ = 1.0;
        primary_->set_can_close(false); // 强制期与验算前不可关
        primary_->set_debug_controls_visible(false);
    }

    apply_primary_geometry();
    if (!lockQss.isEmpty()) primary_->setStyleSheet(lockQss);

    reconcile_screens();
    sync_wall_clocks();
    refresh_ui();
    raise_all(true); // 显示时主屏抢焦点以便输入

    tickTimer_->start();
    watchdogTimer_->start();
    lastTickMs_     = QDateTime::currentMSecsSinceEpoch();
    sleepStartedMs_ = 0;

    if (!visible_) {
        visible_ = true;
        emit visible_changed(); // QML 隐藏主浮窗
    }
}
//------------------------------------------------------
void KtLockScreenManager::sync_force_from_wall_clock() {
    // 由 forceEndMs_ 墙钟推导强制剩余秒数

    if (forceEndMs_ <= 0) {
        isForced_ = false;
        return;
    }

    const int remainingSec = static_cast<int>(
        qCeil((forceEndMs_ - QDateTime::currentMSecsSinceEpoch()) / 1000.0)); // 向上取整秒
    if (remainingSec <= 0) {
        isForced_     = false;
        counterForce_ = 0;
        forceEndMs_   = 0; // 强制期结束
    }
    else {
        isForced_     = true;
        counterForce_ = remainingSec; // 刷新 UI 用
    }
}
//------------------------------------------------------
void KtLockScreenManager::sync_wall_clocks() {
    if (!visible_) return;

    const bool wasRunning = breakClock_.get_running();
    breakClock_.sync_from_wall_clock();

    // 休息倒计时到零：允许关闭窗口，强制期一并结束
    if (wasRunning && breakClock_.get_counter() <= 0) {
        breakClock_.pause();
        if (primary_widget()) primary_widget()->set_can_close(true);
        for (auto* secondary : secondaries_)
            secondary->set_can_close(true);
        isForced_ = false;
    }

    sync_force_from_wall_clock();
    update_formula_visibility();
    refresh_ui();
}
//------------------------------------------------------
void KtLockScreenManager::try_resume_after_wake() {
    if (!visible_) return;

    const qint64 now                 = QDateTime::currentMSecsSinceEpoch();
    const bool   wasSleepSuspended   = breakClock_.is_sleep_suspended();
    const bool   hadSleepStartedMs   = sleepStartedMs_ > 0;

    if (hadSleepStartedMs && forceEndMs_ > 0)
        forceEndMs_ += (now - sleepStartedMs_); // 强制等待期不随休眠流逝

    sleepStartedMs_ = 0;

    if (breakClock_.get_running()) {
        breakClock_.realign_phase_start_if_ahead();
    }
    else if (!breakClock_.resume_after_system_sleep()) {
        if (wasSleepSuspended && breakClock_.get_counter() <= 0)
            sync_wall_clocks();
    }

    if (tickTimer_ && !tickTimer_->isActive())
        tickTimer_->start();

    sync_wall_clocks();
    refresh_ui();

    for (auto* secondary : secondaries_)
        secondary->raise_quiet();
    if (primary_widget()) {
        primary_widget()->raise_quiet();
        primary_widget()->refresh_after_wake();
    }

    lastTickMs_ = now;
    qDebug() << "[LockScreen] try_resume_after_wake breakCounter=" << breakClock_.get_counter()
             << "forced=" << isForced_;
}
//------------------------------------------------------
void KtLockScreenManager::try_unlock(bool /*fromSecondary*/) {
    // 验算 → 更新 canClose → 成功则延迟 clock_out

    if (exiting_ || unlockPending_ || isForced_) return;

    bool canUnlock = false;
    if (breakClock_.get_running()) {
        auto* primaryWidget = primary_widget();
        if (!primaryWidget) return;

        bool      parseOk     = false;
        const int answerValue = primaryWidget->answer_text().toInt(&parseOk);
        canUnlock             = parseOk && (answerValue == formulaValue_); // 比对随机算式
    }
    else {
        canUnlock = true; // 休息已结束，直接允许退出
    }

    if (primary_widget()) {
        primary_widget()->clear_answer();
        primary_widget()->set_can_close(canUnlock);
    }
    for (auto* secondary : secondaries_)
        secondary->set_can_close(canUnlock);

    if (!canUnlock) {
        if (primary_widget()) primary_widget()->set_message_text(tr("Wrong answer!"));
        return;
    }

    if (primary_widget()) primary_widget()->set_message_text(QString());
    unlockPending_ = true;

    // 推迟到事件循环下一轮，避免在 answer_submitted 栈内 destroy UI
    QTimer::singleShot(0, this, [ this ]() {
        if (!exiting_) {
            dismiss();
            emit clock_out(KtAlarmClock::WorkBreak); // 通知 JS 切回工作计时
        }
        unlockPending_ = false;
    });
}
//------------------------------------------------------
void KtLockScreenManager::update_formula_visibility() {
    refresh_ui(); // 与 refresh_ui 合并，避免重复分支
}
