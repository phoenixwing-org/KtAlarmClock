#include "KtTrayService.h"

#include "KtScreenUtil.h"

#include <QAction>
#include <QCursor>
#include <QDebug>
#include <QIcon>
#include <QMenu>

KtTrayService::KtTrayService(QObject* parent)
    : QObject(parent)
    , trayIcon_(nullptr)
    , menu_(nullptr)
    , titleAction_(nullptr)
    , playPauseAction_(nullptr)
    , breakAction_(nullptr)
    , nextLoopAction_(nullptr)
    , settingsAction_(nullptr)
    , helpAction_(nullptr)
    , websiteAction_(nullptr)
    , quitAction_(nullptr)
    , remainingText_(QStringLiteral("0:00"))
    , running_(false)
    , debugLocate_(true)
    , forbidden_(false) {
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning() << "[Tray] system tray not available";
        return;
    }

    trayIcon_ = new QSystemTrayIcon(this);
    trayIcon_->setIcon(QIcon(QStringLiteral(":/image/kt-blue.svg")));
    build_menu();
    update_tooltip();

    connect(trayIcon_, &QSystemTrayIcon::activated, this, &KtTrayService::on_tray_activated);
    trayIcon_->show();
}

KtTrayService::~KtTrayService() {
    hide();
    if (trayIcon_)
        trayIcon_->setContextMenu(nullptr);
    delete menu_;
    menu_ = nullptr;
}

void KtTrayService::build_menu() {
    menu_ = new QMenu();

    titleAction_ = menu_->addAction(QStringLiteral(PROJECT_APP_DISPLAY_NAME));
    titleAction_->setEnabled(false);
    menu_->addSeparator();

    auto addAction = [this](const QString& text, void (KtTrayService::*signalMethod)()) {
        QAction* action = menu_->addAction(text);
        connect(action, &QAction::triggered, this, [this, signalMethod]() {
            emit_if_allowed(signalMethod);
        });
        return action;
    };

    playPauseAction_ = addAction(tr("Start"), &KtTrayService::playPauseRequested);
    breakAction_     = addAction(tr("Break now"), &KtTrayService::breakRequested);
    nextLoopAction_  = addAction(tr("Next loop"), &KtTrayService::nextLoopRequested);
    settingsAction_  = addAction(tr("Settings"), &KtTrayService::settingsRequested);
    menu_->addSeparator();
    helpAction_    = addAction(tr("Help"), &KtTrayService::helpRequested);
    websiteAction_ = addAction(tr("Website"), &KtTrayService::websiteRequested);
    menu_->addSeparator();
    quitAction_ = addAction(tr("Exit"), &KtTrayService::quitRequested);

    if (trayIcon_)
        trayIcon_->setContextMenu(menu_);
    update_menu_state();
}

void KtTrayService::retranslate() {
    if (titleAction_)
        titleAction_->setText(QStringLiteral(PROJECT_APP_DISPLAY_NAME));
    if (breakAction_)
        breakAction_->setText(tr("Break now"));
    if (nextLoopAction_)
        nextLoopAction_->setText(tr("Next loop"));
    if (settingsAction_)
        settingsAction_->setText(tr("Settings"));
    if (helpAction_)
        helpAction_->setText(tr("Help"));
    if (websiteAction_)
        websiteAction_->setText(tr("Website"));
    if (quitAction_)
        quitAction_->setText(tr("Exit"));

    update_menu_state();
    update_tooltip();
}

void KtTrayService::emit_if_allowed(void (KtTrayService::*signalMethod)()) {
    if (forbidden_ && signalMethod != &KtTrayService::quitRequested) {
        if (debugLocate_)
            qDebug() << "[Tray] ignored while forbidden";
        return;
    }
    emit (this->*signalMethod)();
}

void KtTrayService::hide() {
    if (trayIcon_)
        trayIcon_->hide();
}

void KtTrayService::on_tray_activated(QSystemTrayIcon::ActivationReason reason) {
    const QPoint cursorPos = QCursor::pos();
    if (debugLocate_)
        qDebug() << "[Tray] activated reason=" << reason << "cursor=" << cursorPos;

    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        emit_if_allowed(&KtTrayService::playPauseRequested);
        return;
    }

    if (reason == QSystemTrayIcon::Context)
        showContextMenuAt(cursorPos);
}

void KtTrayService::setDebugLocate(bool enabled) {
    debugLocate_ = enabled;
}

void KtTrayService::setForbidden(bool forbidden) {
    if (forbidden_ == forbidden)
        return;
    forbidden_ = forbidden;
    update_menu_state();
}

void KtTrayService::setRemainingText(const QString& text) {
    if (remainingText_ == text)
        return;
    remainingText_ = text;
    update_tooltip();
}

void KtTrayService::setRunning(bool running) {
    if (running_ == running)
        return;
    running_ = running;
    update_menu_state();
    update_tooltip();
}

void KtTrayService::showContextMenuAt(const QPoint& globalPos) {
    if (!menu_)
        return;

    menu_->ensurePolished();
    const QPoint menuPos = KtScreenUtil::place_popup(globalPos, menu_->sizeHint(), true);
    menu_->popup(menuPos);
}

void KtTrayService::update_menu_state() {
    if (playPauseAction_)
        playPauseAction_->setText(running_ ? tr("Pause") : tr("Start"));

    for (QAction* action : {playPauseAction_, breakAction_, nextLoopAction_, settingsAction_}) {
        if (action)
            action->setEnabled(!forbidden_);
    }
}

void KtTrayService::update_tooltip() {
    if (!trayIcon_)
        return;

    const QString stateText = running_ ? tr("running") : tr("paused");
    trayIcon_->setToolTip(QStringLiteral("%1 - %2 (%3)")
                              .arg(QStringLiteral(PROJECT_APP_DISPLAY_NAME), remainingText_, stateText));
}
