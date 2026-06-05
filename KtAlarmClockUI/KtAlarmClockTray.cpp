/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockTray.cpp
 */
#include "KtAlarmClockTray.h"

#include "KtAlarmClock.h"

#include <QAction>
#include <QCursor>
#include <QDebug>
#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>

//------------------------------------------------------
KtAlarmClockTray::KtAlarmClockTray(QObject* parent)
    : QObject(parent)
    , TrayIcon(nullptr)   // 1
    , Menu(nullptr)       // 2
    , debugLocate_(true)  // 3
    , forbidden_(false) { // 4
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning() << "[Tray] system tray not available";
        return;
    }

    TrayIcon = new QSystemTrayIcon(this);
    TrayIcon->setIcon(QIcon(QStringLiteral(":/image/kt-blue.svg")));
    TrayIcon->setToolTip(QStringLiteral("KT护眼闹钟"));

    build_menu();

    connect(TrayIcon, &QSystemTrayIcon::activated, this, &KtAlarmClockTray::on_tray_activated);

    TrayIcon->show();
    if (debugLocate_)
        qDebug() << "[Tray] icon shown";
}
//------------------------------------------------------
KtAlarmClockTray::~KtAlarmClockTray() {
    hide();
    TrayIcon = nullptr; // 1
    Menu     = nullptr; // 2
    // debugLocate_       // 3
    // forbidden_         // 4
}
//------------------------------------------------------
void KtAlarmClockTray::build_menu() {
    Menu = new QMenu();

    auto addAction = [this](const QString& text, int actionId) {
        QAction* action = Menu->addAction(text);
        connect(action, &QAction::triggered, this, [this, actionId]() {
            if (check_forbidden())
                return;
            if (debugLocate_)
                qDebug() << "[Tray] action" << actionId;
            emit action_triggered(actionId);
        });
    };

    addAction(QStringLiteral("KT护眼闹钟"), -1);
    Menu->actions().last()->setEnabled(false); // 标题项不可点
    Menu->addSeparator();
    addAction(QStringLiteral("立刻休息"), KtAlarmClock::ActionBreak);
    addAction(QStringLiteral("显示设置界面"), KtAlarmClock::ActionMainDlg);
    Menu->addSeparator();
    addAction(QStringLiteral("在线帮助"), KtAlarmClock::ActionHelp);
    addAction(QStringLiteral("开源代码主页"), KtAlarmClock::ActionKtWeb);
    Menu->addSeparator();
    addAction(QStringLiteral("Exit"), KtAlarmClock::ActionClose);
}
//------------------------------------------------------
bool KtAlarmClockTray::check_forbidden() const {
    if (forbidden_) {
        if (debugLocate_)
            qDebug() << "[Tray] forbidden (WorkBreak)";
        return true;
    }
    return false;
}
//------------------------------------------------------
void KtAlarmClockTray::hide() {
    if (TrayIcon)
        TrayIcon->hide();
}
//------------------------------------------------------
void KtAlarmClockTray::on_tray_activated(QSystemTrayIcon::ActivationReason reason) {
    const QPoint cursorPos = QCursor::pos();
    if (debugLocate_)
        qDebug() << "[Tray] activated reason=" << reason << "cursor=" << cursorPos;

    if (reason == QSystemTrayIcon::Context && Menu) {
        Menu->popup(cursorPos); // 跟随光标，替代 QML menu.open()
        if (debugLocate_)
            qDebug() << "[Tray] menu.popup at" << cursorPos;
    }
}
