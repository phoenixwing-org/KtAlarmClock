/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtLockScreenPrimaryWidget.cpp
 */

// QT
#include <QCloseEvent>
#include <QDebug>
#include <QEvent>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QResizeEvent>
#include <QScreen>
#include <QShowEvent>
#include <QSvgRenderer>
#include <QTimer>
#include <QVBoxLayout>
#include <QValidator>
#include <QWindow>
#include <QtGlobal>

// LOCAL
#include "KtLockScreenPrimaryWidget.h"
#include "KtMovableWidget.h"

namespace {

constexpr int kCoffeeSize    = 100;
constexpr int kKtBlueSize    = 24;
constexpr int kKtBlueOffsetX = -6; // KT 标相对茶杯图形中心左移
constexpr int kKtBlueOffsetY = 20; // KT 标相对茶杯图形中心下移，落在杯体中间
constexpr int kBreakGap      = 10;
constexpr int kEdgeMargin    = 20;

/**
 * @brief 栅格化 SVG 为 QPixmap
 */
QPixmap render_svg_pixmap(const QString& resourcePath, int width, int height) {
    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);
    QSvgRenderer renderer(resourcePath);
    if (renderer.isValid()) {
        QPainter painter(&pixmap);
        renderer.render(&painter);
    }
    return pixmap;
}

/**
 * @brief 将秒数格式化为 m:ss
 */
QString format_clock(int counter) {
    if (counter <= 0) return QStringLiteral("0:00");
    const int minutePart = counter / 60;
    const int secondPart = counter % 60;
    return QStringLiteral("%1:%2").arg(minutePart).arg(secondPart, 2, 10, QChar('0'));
}

/**
 * @brief 创建带边距限制的可拖动浮层
 */
KtMovableWidget* make_draggable_layer(QWidget* parent) {
    auto* layer = new KtMovableWidget(parent);
    layer->set_clamp_margins(kEdgeMargin);
    return layer;
}

} // namespace

//------------------------------------------------------
KtLockScreenPrimaryWidget::KtLockScreenPrimaryWidget(int screenId, QWidget* parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool)
    , screenId_(screenId)             // 1
    , canClose_(false)                // 2
    , LabelMsg(nullptr)               // 3
    , LabelFormula(nullptr)           // 4
    , EditAnswer(nullptr)             // 5
    , BtnUnlock(nullptr)              // 6
    , UnlockPanel(nullptr)            // 7
    , FormulaRow(nullptr)             // 8
    , LabelBreakTime(nullptr)         // 9
    , LabelForce(nullptr)             // 10
    , ImgCoffee(nullptr)              // 11
    , DebugPanel(nullptr)             // 12
    , BtnDebugExit(nullptr)           // 13
    , BtnDebugSize(nullptr)           // 14
    , DragCoffee(nullptr)             // 15
    , DragKtBlue(nullptr)             // 16
    , DragBreakTime(nullptr)          // 17
    , ImgKtBlue(nullptr)              // 18
    , floatingWidgetsPlaced_(false) { // 19
    setObjectName(QStringLiteral("KtLockScreenPrimary"));
    setFocusPolicy(Qt::StrongFocus);
    build_ui();
    setAttribute(Qt::WA_DeleteOnClose, false);
}
//------------------------------------------------------
KtLockScreenPrimaryWidget::~KtLockScreenPrimaryWidget() {
    LabelMsg       = nullptr; // 3
    LabelFormula   = nullptr; // 4
    EditAnswer     = nullptr; // 5
    BtnUnlock      = nullptr; // 6
    UnlockPanel    = nullptr; // 7
    FormulaRow     = nullptr; // 8
    LabelBreakTime = nullptr; // 9
    LabelForce     = nullptr; // 10
    ImgCoffee      = nullptr; // 11
    DebugPanel     = nullptr; // 12
    BtnDebugExit   = nullptr; // 13
    BtnDebugSize   = nullptr; // 14
    DragCoffee     = nullptr; // 15
    DragKtBlue     = nullptr; // 16
    DragBreakTime  = nullptr; // 17
    ImgKtBlue      = nullptr; // 18
    // floatingWidgetsPlaced_ // 19
}
//------------------------------------------------------
QString KtLockScreenPrimaryWidget::answer_text() const {
    return EditAnswer->text();
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::append_answer_digit(QChar digit) {
    if (!EditAnswer || !digit.isDigit()) return;

    const QString next = EditAnswer->text() + digit;
    if (EditAnswer->maxLength() > 0 && next.length() > EditAnswer->maxLength()) return;

    int pos = 0;
    if (auto* validator = EditAnswer->validator()) {
        QString copy = next;
        if (validator->validate(copy, pos) == QValidator::Invalid) return;
    }

    EditAnswer->setText(next);
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::apply_screen_geometry(bool fullScreen, bool debugMode,
                                                      double debugFraction) {
    const auto screens = QGuiApplication::screens();
    if (screenId_ < 0 || screenId_ >= screens.size()) {
        hide();
        return;
    }

    QScreen*    targetScreen = screens.at(screenId_);
    const QRect screenRect   = targetScreen->geometry();

    if (debugMode) {
        const int width  = qMax(320, static_cast<int>(screenRect.width() * debugFraction));
        const int height = qMax(240, static_cast<int>(screenRect.height() * debugFraction));
        setGeometry(screenRect.x(), screenRect.y(), width, height);
        show();
    }
    else {
        setAttribute(Qt::WA_NativeWindow, true);
        setGeometry(screenRect);
        winId();

        if (QWindow* window = windowHandle()) window->setScreen(targetScreen);

        if (fullScreen)
            showFullScreen();
        else
            show();

        qDebug() << "[LockScreen] primary screenId=" << screenId_ << "name=" << targetScreen->name()
                 << "geometry=" << screenRect;
    }
    raise_quiet();
    if (!floatingWidgetsPlaced_) {
        place_floating_widgets_default();
        floatingWidgetsPlaced_ = true;
    }
    else {
        clamp_floating_widgets();
    }
    raise_floating_widgets();
    QTimer::singleShot(0, this, &KtLockScreenPrimaryWidget::focus_answer_input);
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::build_ui() {
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(20, 20, 20, 20);

    DebugPanel        = new QWidget(this);
    auto* debugLayout = new QHBoxLayout(DebugPanel);
    debugLayout->setContentsMargins(0, 0, 0, 0);

    BtnDebugExit = new QPushButton(tr("调试退出"), DebugPanel);
    BtnDebugExit->setObjectName(QStringLiteral("lockDebugBtn"));
    connect(BtnDebugExit, &QPushButton::clicked, this,
            &KtLockScreenPrimaryWidget::on_debug_exit_clicked);

    BtnDebugSize = new QPushButton(tr("半屏"), DebugPanel);
    BtnDebugSize->setObjectName(QStringLiteral("lockDebugBtn"));
    connect(BtnDebugSize, &QPushButton::clicked, this,
            &KtLockScreenPrimaryWidget::on_debug_size_clicked);

    debugLayout->addWidget(BtnDebugExit);
    debugLayout->addStretch(1);
    debugLayout->addWidget(BtnDebugSize);
    rootLayout->addWidget(DebugPanel, 0, Qt::AlignLeft | Qt::AlignTop);
    DebugPanel->hide();

    UnlockPanel        = new QWidget(this);
    auto* unlockLayout = new QVBoxLayout(UnlockPanel);
    unlockLayout->setContentsMargins(0, 0, 0, 0);

    LabelMsg = new QLabel(UnlockPanel);
    LabelMsg->setObjectName(QStringLiteral("lockMsg"));
    unlockLayout->addWidget(LabelMsg);

    FormulaRow          = new QWidget(UnlockPanel);
    auto* formulaLayout = new QHBoxLayout(FormulaRow);
    formulaLayout->setContentsMargins(0, 0, 0, 0);

    LabelFormula = new QLabel(FormulaRow);
    LabelFormula->setObjectName(QStringLiteral("lockFormula"));
    EditAnswer = new QLineEdit(FormulaRow);
    EditAnswer->setObjectName(QStringLiteral("lockAnswer"));
    EditAnswer->setValidator(new QIntValidator(0, 999999, EditAnswer));
    EditAnswer->setMaxLength(8);
    EditAnswer->setFocusPolicy(Qt::StrongFocus);
    formulaLayout->addWidget(LabelFormula);
    formulaLayout->addWidget(EditAnswer);
    unlockLayout->addWidget(FormulaRow);

    BtnUnlock = new QPushButton(tr("Unlock"), UnlockPanel);
    BtnUnlock->setObjectName(QStringLiteral("lockUnlockBtn"));
    connect(BtnUnlock, &QPushButton::clicked, this, &KtLockScreenPrimaryWidget::on_unlock_clicked);
    connect(EditAnswer, &QLineEdit::returnPressed, this, &KtLockScreenPrimaryWidget::submit_answer);
    unlockLayout->addWidget(BtnUnlock, 0, Qt::AlignLeft);

    rootLayout->addWidget(UnlockPanel, 0, Qt::AlignLeft | Qt::AlignTop);
    rootLayout->addStretch(1);

    // --- 三个独立可拖动浮层 ---
    DragCoffee = make_draggable_layer(this);
    DragCoffee->setFixedSize(kCoffeeSize, kCoffeeSize);
    ImgCoffee = new QLabel(DragCoffee);
    ImgCoffee->setObjectName(QStringLiteral("lockCoffee"));
    ImgCoffee->setPixmap(
        render_svg_pixmap(QStringLiteral(":/image/coffee.svg"), kCoffeeSize, kCoffeeSize));
    ImgCoffee->setFixedSize(kCoffeeSize, kCoffeeSize);
    ImgCoffee->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ImgCoffee->move(0, 0);

    DragKtBlue = make_draggable_layer(this);
    DragKtBlue->setFixedSize(kKtBlueSize, kKtBlueSize);
    ImgKtBlue = new QLabel(DragKtBlue);
    ImgKtBlue->setObjectName(QStringLiteral("lockKtBlue"));
    ImgKtBlue->setPixmap(
        render_svg_pixmap(QStringLiteral(":/image/kt-blue.svg"), kKtBlueSize, kKtBlueSize));
    ImgKtBlue->setFixedSize(kKtBlueSize, kKtBlueSize);
    ImgKtBlue->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ImgKtBlue->move(0, 0);

    DragBreakTime  = make_draggable_layer(this);
    LabelBreakTime = new QLabel(format_clock(0), DragBreakTime);
    LabelBreakTime->setObjectName(QStringLiteral("lockBreakTime"));
    LabelBreakTime->setAlignment(Qt::AlignCenter);
    LabelBreakTime->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    sync_break_time_drag_size();

    LabelForce = new QLabel(this);
    LabelForce->setObjectName(QStringLiteral("lockForceTime"));
    LabelForce->setAlignment(Qt::AlignCenter);
    LabelForce->hide();
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::clamp_floating_widgets() {
    if (DragCoffee) DragCoffee->clamp_to_parent_bounds();
    if (DragKtBlue) DragKtBlue->clamp_to_parent_bounds();
    if (DragBreakTime && DragBreakTime->isVisible()) DragBreakTime->clamp_to_parent_bounds();
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::clear_answer() {
    EditAnswer->clear();
    QTimer::singleShot(0, this, &KtLockScreenPrimaryWidget::focus_answer_input);
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::closeEvent(QCloseEvent* event) {
    if (!canClose_)
        event->ignore();
    else
        QWidget::closeEvent(event);
}
//------------------------------------------------------
bool KtLockScreenPrimaryWidget::event(QEvent* event) {
    if (event->type() == QEvent::WindowDeactivate && isVisible()) {
        QTimer::singleShot(0, this, [ this ]() {
            if (isVisible()) raise_quiet();
        });
    }
    return QWidget::event(event);
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::focus_answer_input() {
    if (!UnlockPanel || !UnlockPanel->isVisible() || !FormulaRow || !FormulaRow->isVisible() ||
        !EditAnswer)
        return;

    activateWindow();
    raise();
    EditAnswer->setFocus(Qt::OtherFocusReason);
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::keyPressEvent(QKeyEvent* event) {
    const bool canAnswer =
        UnlockPanel && UnlockPanel->isVisible() && FormulaRow && FormulaRow->isVisible();

    if (!canAnswer) {
        QWidget::keyPressEvent(event);
        return;
    }

    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        submit_answer();
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Backspace) {
        const QString text = EditAnswer->text();
        if (!text.isEmpty()) EditAnswer->setText(text.left(text.length() - 1));
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Space) {
        submit_answer();
        event->accept();
        return;
    }

    if (event->text().length() == 1) {
        const QChar inputChar = event->text().at(0);
        if (inputChar.isDigit()) {
            append_answer_digit(inputChar);
            event->accept();
            return;
        }
        submit_answer();
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::on_debug_exit_clicked() {
    emit debug_exit_requested();
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::on_debug_size_clicked() {
    emit debug_size_toggle_requested();
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::on_unlock_clicked() {
    submit_answer();
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::place_floating_widgets_default() {
    const QRect bounds = rect().adjusted(kEdgeMargin, kEdgeMargin, -kEdgeMargin, -kEdgeMargin);
    if (!bounds.isValid()) return;

    sync_break_time_drag_size();

    const int centerX = bounds.center().x();
    const int centerY = bounds.center().y();

    if (DragCoffee) DragCoffee->move(centerX - kCoffeeSize / 2, centerY - kCoffeeSize / 2);
    if (DragKtBlue) {
        const int ktX = centerX - kKtBlueSize / 2 + kKtBlueOffsetX;
        const int ktY = centerY - kKtBlueSize / 2 + kKtBlueOffsetY;
        DragKtBlue->move(ktX, ktY);
    }
    if (DragBreakTime) {
        const int breakX = centerX - DragBreakTime->width() / 2;
        const int breakY = centerY - kCoffeeSize / 2 + kCoffeeSize + kBreakGap;
        DragBreakTime->move(breakX, breakY);
    }

    clamp_floating_widgets();
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::raise_floating_widgets() {
    if (DragCoffee) DragCoffee->raise();
    if (DragKtBlue) DragKtBlue->raise();
    if (DragBreakTime) DragBreakTime->raise();
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::raise_quiet() {
    raise();
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::refresh_after_wake() {
    clamp_floating_widgets();
    raise_floating_widgets();
    QTimer::singleShot(150, this, &KtLockScreenPrimaryWidget::focus_answer_input);
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    clamp_floating_widgets();
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::set_break_time_text(const QString& text) {
    LabelBreakTime->setText(text);
    LabelBreakTime->setVisible(!text.isEmpty());
    DragBreakTime->setVisible(!text.isEmpty());
    sync_break_time_drag_size();
    clamp_floating_widgets();
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::set_can_close(bool canClose) {
    canClose_ = canClose;
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::set_debug_controls_visible(bool visible) {
    if (DebugPanel) DebugPanel->setVisible(visible);
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::set_force_visible(bool visible, int seconds) {
    Q_UNUSED(visible);
    Q_UNUSED(seconds);
    LabelForce->hide();
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::set_formula_text(const QString& text) {
    LabelFormula->setText(text);
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::set_formula_visible(bool visible) {
    FormulaRow->setVisible(visible);
    if (visible) QTimer::singleShot(0, this, &KtLockScreenPrimaryWidget::focus_answer_input);
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::set_message_text(const QString& text) {
    LabelMsg->setText(text);
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::set_unlock_panel_visible(bool visible) {
    UnlockPanel->setVisible(visible);
    if (visible) QTimer::singleShot(0, this, &KtLockScreenPrimaryWidget::focus_answer_input);
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    if (!floatingWidgetsPlaced_) {
        place_floating_widgets_default();
        floatingWidgetsPlaced_ = true;
    }
    else {
        clamp_floating_widgets();
    }
    raise_floating_widgets();
    QTimer::singleShot(0, this, &KtLockScreenPrimaryWidget::focus_answer_input);
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::submit_answer() {
    emit answer_submitted();
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::sync_break_time_drag_size() {
    if (!DragBreakTime || !LabelBreakTime) return;

    LabelBreakTime->adjustSize();
    DragBreakTime->setFixedSize(LabelBreakTime->size());
    LabelBreakTime->move(0, 0);
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::update_debug_size_button_text(double debugFraction) {
    if (!BtnDebugSize) return;
    if (debugFraction >= 1.0)
        BtnDebugSize->setText(tr("半屏"));
    else if (debugFraction <= 0.25)
        BtnDebugSize->setText(tr("半屏"));
    else
        BtnDebugSize->setText(tr("全屏"));
}
