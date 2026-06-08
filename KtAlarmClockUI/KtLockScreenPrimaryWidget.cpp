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
#include <QScreen>
#include <QSvgRenderer>
#include <QVBoxLayout>
#include <QWindow>
#include <QtGlobal>

// LOCAL
#include "KtLockScreenPrimaryWidget.h"

/**
 * @brief 将秒数格式化为 m:ss
 * @param counter 秒数
 * @return 格式化后的字符串
 */
static QString format_clock(int counter) {
    if (counter <= 0) return QStringLiteral("0:00"); // 到期显示 0:00
    const int minutePart = counter / 60;             // 分
    const int secondPart = counter % 60;             // 秒
    return QStringLiteral("%1:%2").arg(minutePart).arg(secondPart, 2, 10, QChar('0'));
}

//------------------------------------------------------
KtLockScreenPrimaryWidget::KtLockScreenPrimaryWidget(int screenId, QWidget* parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool)
    , screenId_(screenId)                                 // 1
    , canClose_(false)                                    // 2
    , LabelMsg(nullptr)                                   // 3
    , LabelFormula(nullptr)                               // 4
    , EditAnswer(nullptr)                                 // 5
    , BtnUnlock(nullptr)                                  // 6
    , UnlockPanel(nullptr)                                // 7
    , FormulaRow(nullptr)                                 // 8
    , LabelBreakTime(nullptr)                             // 9
    , LabelForce(nullptr)                                 // 10
    , ImgCoffee(nullptr)                                  // 11
    , DebugPanel(nullptr)                                 // 12
    , BtnDebugExit(nullptr)                               // 13
    , BtnDebugSize(nullptr) {                             // 14
    setObjectName(QStringLiteral("KtLockScreenPrimary")); // QSS 选择器
    build_ui();
    setAttribute(Qt::WA_DeleteOnClose, false); // 由 Manager 复用，不随 close 销毁
}
//------------------------------------------------------
KtLockScreenPrimaryWidget::~KtLockScreenPrimaryWidget() {
    // screenId_ = 0; // 1
    // canClose_ = false; // 2
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
}
//------------------------------------------------------
QString KtLockScreenPrimaryWidget::answer_text() const {
    return EditAnswer->text(); // 供 Manager 验算
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::apply_screen_geometry(bool fullScreen, bool debugMode,
                                                      double debugFraction) {
    // 铺满指定屏幕；debug 下按占屏比左上角显示

    const auto screens = QGuiApplication::screens();
    if (screenId_ < 0 || screenId_ >= screens.size()) {
        hide(); // 目标屏已消失，避免残留在剩余显示器上
        return;
    }

    QScreen*       targetScreen = screens.at(screenId_);
    const QRect    screenRect   = targetScreen->geometry();

    if (debugMode) {
        const int width  = qMax(320, static_cast<int>(screenRect.width() * debugFraction));
        const int height = qMax(240, static_cast<int>(screenRect.height() * debugFraction));
        setGeometry(screenRect.x(), screenRect.y(), width, height);
        show(); // debug 不用 showFullScreen，留桌面可操作
    } else {
        setAttribute(Qt::WA_NativeWindow, true);
        setGeometry(screenRect);
        winId();

        if (QWindow* window = windowHandle())
            window->setScreen(targetScreen);

        if (fullScreen)
            showFullScreen();
        else
            show();

        qDebug() << "[LockScreen] primary screenId=" << screenId_
                 << "name=" << targetScreen->name() << "geometry=" << screenRect;
    }
    raise_quiet(); // 显示后立即置顶
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::build_ui() {
    // 布局：左上解锁区 + 中央茶杯与倒计时

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(20, 20, 20, 20);

    // --- 调试栏：退出 / 半屏·全屏切换（仅 debug 显示） ---
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

    // --- 解锁区：提示 / 算式 / 按钮 ---
    UnlockPanel        = new QWidget(this);
    auto* unlockLayout = new QVBoxLayout(UnlockPanel);
    unlockLayout->setContentsMargins(0, 0, 0, 0);

    LabelMsg = new QLabel(UnlockPanel);
    LabelMsg->setObjectName(QStringLiteral("lockMsg")); // 答错提示等
    unlockLayout->addWidget(LabelMsg);

    FormulaRow          = new QWidget(UnlockPanel);
    auto* formulaLayout = new QHBoxLayout(FormulaRow);
    formulaLayout->setContentsMargins(0, 0, 0, 0);

    LabelFormula = new QLabel(FormulaRow);
    LabelFormula->setObjectName(QStringLiteral("lockFormula"));
    EditAnswer = new QLineEdit(FormulaRow);
    EditAnswer->setObjectName(QStringLiteral("lockAnswer"));
    EditAnswer->setValidator(new QIntValidator(0, 999999, EditAnswer)); // 仅非负整数
    EditAnswer->setMaxLength(8);
    formulaLayout->addWidget(LabelFormula);
    formulaLayout->addWidget(EditAnswer);
    unlockLayout->addWidget(FormulaRow);

    BtnUnlock = new QPushButton(tr("Unlock"), UnlockPanel);
    BtnUnlock->setObjectName(QStringLiteral("lockUnlockBtn"));
    connect(BtnUnlock, &QPushButton::clicked, this, &KtLockScreenPrimaryWidget::on_unlock_clicked);
    connect(EditAnswer, &QLineEdit::returnPressed, this, &KtLockScreenPrimaryWidget::submit_answer);
    unlockLayout->addWidget(BtnUnlock, 0, Qt::AlignLeft);

    rootLayout->addWidget(UnlockPanel, 0, Qt::AlignLeft | Qt::AlignTop);

    // --- 中央：SVG 茶杯 + 休息倒计时 + 强制秒数 ---
    auto* centerWidget = new QWidget(this);
    auto* centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setAlignment(Qt::AlignCenter);

    ImgCoffee = new QLabel(centerWidget);
    ImgCoffee->setObjectName(QStringLiteral("lockCoffee"));
    QPixmap coffeePix(100, 100);
    coffeePix.fill(Qt::transparent);
    QSvgRenderer coffeeSvg(QStringLiteral(":/image/coffee.svg"));
    if (coffeeSvg.isValid()) {
        QPainter painter(&coffeePix);
        coffeeSvg.render(&painter); // 栅格化 SVG 供 QLabel 显示
    }
    ImgCoffee->setPixmap(coffeePix);
    ImgCoffee->setFixedSize(100, 100);
    centerLayout->addWidget(ImgCoffee, 0, Qt::AlignHCenter);

    LabelBreakTime = new QLabel(format_clock(0), centerWidget);
    LabelBreakTime->setObjectName(QStringLiteral("lockBreakTime"));
    LabelBreakTime->setAlignment(Qt::AlignCenter);
    centerLayout->addWidget(LabelBreakTime, 0, Qt::AlignHCenter);

    LabelForce = new QLabel(centerWidget);
    LabelForce->setObjectName(QStringLiteral("lockForceTime"));
    LabelForce->setAlignment(Qt::AlignCenter);
    LabelForce->hide(); // 默认隐藏，强制期由 Manager 显示
    centerLayout->addWidget(LabelForce, 0, Qt::AlignHCenter);

    rootLayout->addStretch(1);
    rootLayout->addWidget(centerWidget, 0, Qt::AlignCenter);
    rootLayout->addStretch(2); // 中央区视觉居中
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::clear_answer() {
    EditAnswer->clear(); // 验算后清空，防重复提交
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::closeEvent(QCloseEvent* event) {
    if (!canClose_)
        event->ignore(); // 未到点或答错时禁止关闭
    else
        QWidget::closeEvent(event);
}
//------------------------------------------------------
bool KtLockScreenPrimaryWidget::event(QEvent* event) {
    // 失焦时静默 raise，保持遮罩在最前但不抢焦点
    if (event->type() == QEvent::WindowDeactivate && isVisible()) raise_quiet();
    return QWidget::event(event);
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::keyPressEvent(QKeyEvent* event) {
    // 空格 / 回车视为提交答案
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return ||
        event->key() == Qt::Key_Enter) {
        submit_answer();
        event->accept();
        return;
    }

    // 单字符：数字交给 LineEdit，其余键视为提交（对齐原 QML 行为）
    if (event->text().length() == 1) {
        const QChar inputChar = event->text().at(0);
        if (inputChar.isDigit()) {
            QWidget::keyPressEvent(event); // 继续输入
            return;
        }
        submit_answer();
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event); // 其他键默认处理
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::on_debug_exit_clicked() {
    emit debug_exit_requested(); // 由 Manager 直接 hide
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::on_debug_size_clicked() {
    emit debug_size_toggle_requested();
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::on_unlock_clicked() {
    submit_answer(); // 与键盘提交共用逻辑
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::raise_quiet() {
    raise(); // 不调用 activateWindow
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::set_can_close(bool canClose) {
    canClose_ = canClose; // 控制 closeEvent 是否 ignore
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::set_debug_controls_visible(bool visible) {
    if (DebugPanel)
        DebugPanel->setVisible(visible);
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::set_break_time_text(const QString& text) {
    LabelBreakTime->setText(text);
    LabelBreakTime->setVisible(!text.isEmpty()); // 休息结束后隐藏
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::set_force_visible(bool visible, int seconds) {
    Q_UNUSED(visible);
    Q_UNUSED(seconds);
    LabelForce->hide(); // 与 MyOver0.qml 一致：主屏只显示休息总倒计时
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::set_formula_text(const QString& text) {
    LabelFormula->setText(text); // 如 "123 + 456 ="
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::set_formula_visible(bool visible) {
    FormulaRow->setVisible(visible); // 强制期或休息结束隐藏
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::set_message_text(const QString& text) {
    LabelMsg->setText(text); // Wrong answer! 等
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::set_unlock_panel_visible(bool visible) {
    UnlockPanel->setVisible(visible); // 强制期整块隐藏
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::update_debug_size_button_text(double debugFraction) {
    if (!BtnDebugSize)
        return;
    if (debugFraction >= 1.0)
        BtnDebugSize->setText(tr("半屏"));
    else if (debugFraction <= 0.25)
        BtnDebugSize->setText(tr("半屏")); // 1/4 首次进入半屏
    else
        BtnDebugSize->setText(tr("全屏")); // 半屏 → 全屏
}
//------------------------------------------------------
void KtLockScreenPrimaryWidget::submit_answer() {
    emit answer_submitted(); // 由 Manager 验算
}
