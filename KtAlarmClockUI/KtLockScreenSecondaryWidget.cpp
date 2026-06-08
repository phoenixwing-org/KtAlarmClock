/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtLockScreenSecondaryWidget.cpp
 */
#include "KtLockScreenSecondaryWidget.h"

#include <QCloseEvent>
#include <QDebug>
#include <QGuiApplication>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QScreen>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWindow>

namespace {
constexpr int kMessageHorizontalMargin = 40; // 与 QML width: parent.width - 40 一致
}

//------------------------------------------------------
KtLockScreenSecondaryWidget::KtLockScreenSecondaryWidget(int screenId, QWidget* parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool)
    , screenId_(screenId)   // 1
    , canClose_(false)      // 2
    , LabelMessage(nullptr) // 3
    , LabelForce(nullptr)   // 4
    , BtnUnlock(nullptr) {  // 5
    setObjectName(QStringLiteral("KtLockScreenSecondary"));
    build_ui();
}
//------------------------------------------------------
KtLockScreenSecondaryWidget::~KtLockScreenSecondaryWidget() {
    // screenId_   // 1
    // canClose_   // 2
    LabelMessage = nullptr; // 3
    LabelForce   = nullptr; // 4
    BtnUnlock    = nullptr; // 5
}
//------------------------------------------------------
void KtLockScreenSecondaryWidget::apply_screen_geometry(bool fullScreen, bool debugMode,
                                                        const QWidget* anchor) {
    // 按目标屏幕全屏；调试时在主窗旁开小窗

    const auto screens = QGuiApplication::screens();
    if (screenId_ < 0 || screenId_ >= screens.size()) {
        hide(); // 目标屏已消失
        return;
    }

    QScreen* targetScreen = screens.at(screenId_);
    if (debugMode && anchor) {
        // 单屏开发：副屏贴主窗右侧，灰色背景便于区分
        setGeometry(anchor->x() + anchor->width(), anchor->y(), 640, 480);
        setStyleSheet(QStringLiteral("background-color: grey;"));
        show();
    }
    else {
        const QRect screenRect = targetScreen->geometry();
        setAttribute(Qt::WA_NativeWindow, true);
        setGeometry(screenRect);
        winId();

        if (QWindow* window = windowHandle())
            window->setScreen(targetScreen); // 遮罩必须挂到目标显示器

        if (fullScreen)
            showFullScreen();
        else
            show();

        qDebug() << "[LockScreen] secondary screenId=" << screenId_
                 << "name=" << targetScreen->name() << "geometry=" << screenRect;
    }
    update_message_layout(); // showFullScreen 后才有正确宽度
    raise_quiet();
}
//------------------------------------------------------
void KtLockScreenSecondaryWidget::build_ui() {
    // 布局：顶 Unlock → 中文案 → 强制倒计时（默认隐藏）

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(20, 20, 20, 20);

    BtnUnlock = new QPushButton(tr("Unlock"), this);
    BtnUnlock->setObjectName(QStringLiteral("lockUnlockBtn"));
    connect(BtnUnlock, &QPushButton::clicked, this, &KtLockScreenSecondaryWidget::unlock_requested);
    rootLayout->addWidget(BtnUnlock, 0, Qt::AlignLeft | Qt::AlignTop);

    rootLayout->addStretch(1); // 将文案挤到视觉中央

    LabelMessage = new QLabel(tr("Have a rest! Except work, there are poetry and afar!"), this);
    LabelMessage->setObjectName(QStringLiteral("lockSecondaryMsg"));
    LabelMessage->setWordWrap(true);
    LabelMessage->setAlignment(Qt::AlignCenter);
    LabelMessage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    LabelMessage->setVisible(true); // 副屏文案从进入锁屏起即显示
    rootLayout->addWidget(LabelMessage, 0, Qt::AlignHCenter);

    LabelForce = new QLabel(this);
    LabelForce->setObjectName(QStringLiteral("lockForceTime"));
    LabelForce->setAlignment(Qt::AlignCenter);
    LabelForce->hide(); // 强制期由 set_forced 显示
    rootLayout->addWidget(LabelForce, 0, Qt::AlignCenter);

    rootLayout->addStretch(2);
}
//------------------------------------------------------
void KtLockScreenSecondaryWidget::closeEvent(QCloseEvent* event) {
    if (!canClose_)
        event->ignore(); // 强制期或未到点不可关
    else
        QWidget::closeEvent(event);
}
//------------------------------------------------------
void KtLockScreenSecondaryWidget::raise_quiet() {
    raise(); // 仅置顶，不 activateWindow
}
//------------------------------------------------------
void KtLockScreenSecondaryWidget::set_can_close(bool canClose) {
    canClose_ = canClose; // 控制 closeEvent 是否 ignore
}
//------------------------------------------------------
void KtLockScreenSecondaryWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    update_message_layout();
}
//------------------------------------------------------
void KtLockScreenSecondaryWidget::set_forced(bool forced, int counterForce) {
    Q_UNUSED(counterForce);
    // 强制期仅隐藏 Unlock；文案始终可见（与主屏强制期不同）

    LabelForce->hide();
    LabelMessage->setVisible(true);
    BtnUnlock->setVisible(!forced);
}
//------------------------------------------------------
void KtLockScreenSecondaryWidget::update_message_layout() {
    if (!LabelMessage || width() <= 0)
        return;

    const int textWidth = qMax(200, width() - kMessageHorizontalMargin);
    LabelMessage->setFixedWidth(textWidth); // WordWrap 需要明确宽度
    LabelMessage->adjustSize();
}
