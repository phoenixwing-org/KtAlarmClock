/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtMainClockWidget.cpp
 */
#include "KtMainClockWidget.h"

#include <QCloseEvent>
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
    , clampingMove_(false) {   // 8
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
    TickTimer->setInterval(1000);
    connect(TickTimer, &QTimer::timeout, this, &KtMainClockWidget::on_tick);

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
void KtMainClockWidget::ensure_within_screen() {
    apply_clamped_position(frameGeometry().topLeft());
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
void KtMainClockWidget::clock_pause() {
    workClock_.pause();
    TickTimer->stop();
    refresh_display();
}
//------------------------------------------------------
void KtMainClockWidget::clock_start(int state, int counterSec) {
    workClock_.start(state, counterSec);
    if (workClock_.get_running())
        TickTimer->start();
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
    sync_from_wall_clock();
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
