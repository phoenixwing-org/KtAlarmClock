/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtMainClockWidget.cpp
 */
#include "KtMainClockWidget.h"

#include "KtAlarmClock.h"
#include "KtWindowsWakeNotifier.h"

#include <QCloseEvent>
#include <QDateTime>
#include <QDebug>
#include <QFont>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QShowEvent>
#include <QTimer>
#include <QWindow>

namespace {
constexpr int kMinWidth   = 80;
constexpr int kMinHeight  = 34;
constexpr int kHPad       = 20;
constexpr int kTimePointSize = 12; // 点字号随 DPI 缩放，比固定 pixel 更清晰
// 真休眠/合盖睡眠时 tick 通常停很久；阈值过小会把 UI 卡顿误判为休眠并冻结计时
constexpr int kSleepGapMs = 12000;
} // namespace

//------------------------------------------------------
KtMainClockWidget::KtMainClockWidget(QWidget* parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool)
    , workClock_()           // 1
    , TimeLabel(nullptr)     // 2
    , TickTimer(nullptr)     // 3
    , canClose_(false)       // 4
    , dragging_(false)         // 5
    , dragOffset_()            // 6
    , initialCounter_(-100)    // 7
    , clampingMove_(false)   // 8
    , lastTickMs_(0)          // 9
    , WakeWatchTimer(nullptr) { // 10
    setAttribute(Qt::WA_TranslucentBackground, true); // 圆角胶囊需透明底
    setMinimumSize(kMinWidth, kMinHeight);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(kHPad / 2, 4, kHPad / 2, 4);

    TimeLabel = new QLabel(QStringLiteral("0:00"), this);
    QFont font(QStringLiteral("Segoe UI"));
    font.setPointSize(kTimePointSize);
    TimeLabel->setFont(font);
    TimeLabel->setStyleSheet(QStringLiteral("color: #46aef6; background: transparent;"));
    layout->addWidget(TimeLabel, 0, Qt::AlignCenter);

    workClock_.reset(0, initialCounter_); // 未启动：counter = -100

    TickTimer = new QTimer(this);
    TickTimer->setTimerType(Qt::PreciseTimer);
    TickTimer->setInterval(1000);
    connect(TickTimer, &QTimer::timeout, this, &KtMainClockWidget::on_tick);

    WakeWatchTimer = new QTimer(this);
    WakeWatchTimer->setInterval(1000);
    connect(WakeWatchTimer, &QTimer::timeout, this, &KtMainClockWidget::on_wake_watchdog);
    WakeWatchTimer->start();

    auto* wakeNotifier = new KtWindowsWakeNotifier(this);
    connect(wakeNotifier, &KtWindowsWakeNotifier::system_resumed, this,
            &KtMainClockWidget::try_resume_after_wake);

    move(600, 2); // 与 QML root.x 默认一致
    refresh_display();
}
//------------------------------------------------------
KtMainClockWidget::~KtMainClockWidget() {
    TimeLabel = nullptr; // 2
    TickTimer = nullptr; // 3
    // workClock_            // 1
    // canClose_             // 4
    // dragging_             // 5
    // dragOffset_           // 6
    // initialCounter_       // 7
    // clampingMove_         // 8
    // lastTickMs_           // 9
}
//------------------------------------------------------
void KtMainClockWidget::apply_clamped_position(const QPoint& globalTopLeft) {
    if (clampingMove_)
        return;

    const QPoint clamped = clamp_to_screen(globalTopLeft);
    QScreen*     screen  = screen_for_position(clamped);

    clampingMove_ = true;
    if (screen) {
        if (auto* window = windowHandle()) {
            if (window->screen() != screen)
                window->setScreen(screen); // 跨 DPI 屏前先绑定目标 screen
        }
    }
    if (pos() != clamped)
        move(clamped);
    clampingMove_ = false;
}
//------------------------------------------------------
QPoint KtMainClockWidget::clamp_to_screen(const QPoint& globalTopLeft) const {
    QScreen* screen = screen_for_position(globalTopLeft);
    if (!screen)
        return globalTopLeft;

    const QRect geo      = screen->availableGeometry();
    const int   widgetW  = width();
    const int   widgetH  = height();
    int         posX     = globalTopLeft.x();
    int         posY     = globalTopLeft.y();

    if (posX < geo.left())
        posX = geo.left(); // 左边界
    if (posY < geo.top())
        posY = geo.top(); // 上边界
    if (posX + widgetW > geo.right() + 1)
        posX = geo.right() + 1 - widgetW; // 右边界
    if (posY + widgetH > geo.bottom() + 1)
        posY = geo.bottom() + 1 - widgetH; // 下边界

    if (widgetW > geo.width())
        posX = geo.left();
    if (widgetH > geo.height())
        posY = geo.top();

    return QPoint(posX, posY);
}
//------------------------------------------------------
void KtMainClockWidget::clock_pause() {
    workClock_.pause();
    TickTimer->stop();
    refresh_display();
}
//------------------------------------------------------
void KtMainClockWidget::clock_start(int state, int counterSec, bool leanFirstSecond) {
    workClock_.start(state, counterSec, leanFirstSecond);
    if (workClock_.get_running())
        ensure_work_tick_running();
    else
        TickTimer->stop();
    refresh_display();
}
//------------------------------------------------------
void KtMainClockWidget::closeEvent(QCloseEvent* event) {
    if (!canClose_)
        event->ignore(); // 默认禁止用户直接关窗
    else
        QWidget::closeEvent(event);
}
//------------------------------------------------------
void KtMainClockWidget::detect_sleep_gap() {
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (lastTickMs_ <= 0) {
        lastTickMs_ = now;
        return;
    }

    if (!workClock_.get_running() || workClock_.get_state() != KtAlarmClock::WorkTime) {
        lastTickMs_ = now;
        return;
    }

    if (now - lastTickMs_ > kSleepGapMs) {
        workClock_.freeze_for_system_sleep_at(lastTickMs_);
        TickTimer->stop();
    }

    lastTickMs_ = now;
}
//------------------------------------------------------
void KtMainClockWidget::ensure_within_screen() {
    apply_clamped_position(frameGeometry().topLeft());
}
//------------------------------------------------------
void KtMainClockWidget::ensure_work_tick_running() {
    if (!TickTimer || !workClock_.get_running()) {
        if (TickTimer)
            TickTimer->stop();
        return;
    }

    lastTickMs_ = QDateTime::currentMSecsSinceEpoch();
    on_tick(); // 点击播放/下一个后立即刷新，不等首个 1s 定时器
    TickTimer->start();
}
//------------------------------------------------------
QString KtMainClockWidget::format_time(int counterSec) {
    if (counterSec <= 0)
        return QStringLiteral("0:00");
    const int minutePart = counterSec / 60;
    const int secondPart = counterSec % 60;
    return QStringLiteral("%1:%2").arg(minutePart).arg(secondPart, 2, 10, QChar('0'));
}
//------------------------------------------------------
int KtMainClockWidget::get_counter() const {
    return workClock_.get_counter();
}
//------------------------------------------------------
bool KtMainClockWidget::get_running() const {
    return workClock_.get_running();
}
//------------------------------------------------------
void KtMainClockWidget::handle_application_active() {
    try_resume_after_wake();
}
//------------------------------------------------------
void KtMainClockWidget::handle_application_suspended() {
    if (workClock_.get_running() && workClock_.get_state() == KtAlarmClock::WorkTime) {
        workClock_.enter_system_sleep();
        TickTimer->stop();
    }
    lastTickMs_ = QDateTime::currentMSecsSinceEpoch();
    refresh_display();
}
//------------------------------------------------------
void KtMainClockWidget::mouseMoveEvent(QMouseEvent* event) {
    if (dragging_ && (event->buttons() & Qt::LeftButton)) {
        apply_clamped_position(event->globalPos() - dragOffset_);
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}
//------------------------------------------------------
void KtMainClockWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton) {
        qDebug() << "[MainClock] right-click globalPos=" << event->globalPos();
        emit context_menu_requested(event->globalPos());
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        dragging_   = true;
        dragOffset_ = event->globalPos() - frameGeometry().topLeft();
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}
//------------------------------------------------------
void KtMainClockWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton)
        dragging_ = false;
    QWidget::mouseReleaseEvent(event);
}
//------------------------------------------------------
void KtMainClockWidget::on_tick() {
    detect_sleep_gap();
    sync_from_wall_clock();
}
//------------------------------------------------------
void KtMainClockWidget::on_wake_watchdog() {
    const qint64 now = QDateTime::currentMSecsSinceEpoch();

    if (workClock_.is_sleep_suspended()) {
        // 进程定时器恢复跑、且距末次 tick 已久 → 认定机器已唤醒
        if (lastTickMs_ > 0 && now - lastTickMs_ >= kSleepGapMs)
            try_resume_after_wake();
        return;
    }

    if (workClock_.get_running() && workClock_.get_state() == KtAlarmClock::WorkTime) {
        workClock_.realign_phase_start_if_ahead();
        if (TickTimer && !TickTimer->isActive())
            ensure_work_tick_running();
    }
}
//------------------------------------------------------
void KtMainClockWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(QColor(QStringLiteral("#222222")));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), height() / 2.0, height() / 2.0); // 胶囊圆角
}
//------------------------------------------------------
void KtMainClockWidget::refresh_display() {
    TimeLabel->setText(format_time(workClock_.get_counter()));
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    const int textW = TimeLabel->fontMetrics().horizontalAdvance(TimeLabel->text());
#else
    const int textW = TimeLabel->fontMetrics().width(TimeLabel->text());
#endif
    const int w = qMax(kMinWidth, textW + kHPad);
    const int h = qMax(kMinHeight, TimeLabel->fontMetrics().height() + 10);
    if (width() != w || height() != h)
        resize(w, h); // 随文本宽度伸缩
    ensure_within_screen(); // resize 后防止右侧/下侧越界
}
//------------------------------------------------------
QScreen* KtMainClockWidget::screen_for_position(const QPoint& globalTopLeft) const {
    const QRect widgetRect(globalTopLeft.x(), globalTopLeft.y(), width(), height());
    QScreen*    screen     = QGuiApplication::screenAt(widgetRect.center());
    if (!screen)
        screen = QGuiApplication::screenAt(globalTopLeft);
    if (!screen && windowHandle())
        screen = windowHandle()->screen();
    if (!screen)
        screen = QGuiApplication::primaryScreen();
    return screen;
}
//------------------------------------------------------
void KtMainClockWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    if (auto* window = windowHandle()) {
        connect(window, &QWindow::screenChanged, this, [this](QScreen*) {
            ensure_within_screen(); // 拖到另一块屏后按新 DPI 重新夹紧
        }, Qt::UniqueConnection);
    }
    ensure_within_screen();
}
//------------------------------------------------------
void KtMainClockWidget::sync_from_wall_clock() {
    if (!workClock_.get_running())
        return;
    workClock_.sync_from_wall_clock();
    refresh_display();
    if (workClock_.is_expired())
        emit clock_out(workClock_.get_state());
}
//------------------------------------------------------
void KtMainClockWidget::try_resume_after_wake() {
    const bool wasSleepSuspended = workClock_.is_sleep_suspended();

    if (workClock_.get_running()) {
        workClock_.realign_phase_start_if_ahead();
        ensure_work_tick_running();
        return;
    }

    if (!workClock_.resume_after_system_sleep()) {
        if (wasSleepSuspended && workClock_.get_counter() <= 0)
            emit clock_out(workClock_.get_state());
        lastTickMs_ = QDateTime::currentMSecsSinceEpoch();
        refresh_display();
        return;
    }

    ensure_work_tick_running();
}
