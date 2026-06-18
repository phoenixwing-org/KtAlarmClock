#include "KtLockScreenManager.h"

#include "KtAlarmClock.h"
#include "KtQpcClock.h"

#include <QCloseEvent>
#include <QCoreApplication>
#include <QDebug>
#include <QEvent>
#include <QGuiApplication>
#include <QMetaObject>
#include <QQmlError>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickView>
#include <QRandomGenerator>
#include <QScreen>
#include <QTimer>
#include <QWindow>
#include <QtMath>

namespace {

QString format_clock(int counter) {
    if (counter <= 0)
        return QStringLiteral("0:00");

    const int minutePart = counter / 60;
    const int secondPart = counter % 60;
    return QStringLiteral("%1:%2").arg(minutePart).arg(secondPart, 2, 10, QChar('0'));
}

QString debug_size_label(double fraction) {
    if (fraction <= 0.25)
        return QStringLiteral("Half");
    return fraction >= 1.0 ? QStringLiteral("Half") : QStringLiteral("Full");
}

} // namespace

//----------------------------------------
KtLockScreenManager::KtLockScreenManager(QObject* parent)
    : QObject(parent)
    , breakClock_()
    , audioMuteGuard_()
    , keyboardBlocker_()
    , primary_(nullptr)
    , tickTimer_(new QTimer(this))
    , watchdogTimer_(new QTimer(this))
    , visible_(false)
    , exiting_(false)
    , debugMode_(false)
    , unlockPending_(false)
    , isForced_(false)
    , canClose_(false)
    , counterForce_(0)
    , forceEndMs_(0)
    , formulaValue_(0)
    , formulaText_()
    , unlockFailureSerial_(0)
    , primaryScreenId_(0)
    , secondaries_()
    , debugSizeFraction_(1.0) {
    tickTimer_->setInterval(1000);
    connect(tickTimer_, &QTimer::timeout, this, &KtLockScreenManager::on_tick);

    watchdogTimer_->setInterval(10000);
    connect(watchdogTimer_, &QTimer::timeout, this, &KtLockScreenManager::on_watchdog);

    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this,
            &KtLockScreenManager::on_application_state_changed);
    connect(qGuiApp, &QGuiApplication::screenAdded, this, [this]() {
        if (visible_)
            reconcile_screens();
    });
    connect(qGuiApp, &QGuiApplication::screenRemoved, this, [this](QScreen*) {
        if (visible_)
            reconcile_screens();
    });
}
//----------------------------------------
KtLockScreenManager::~KtLockScreenManager() {
    hide();

    if (primary_) {
        primary_->removeEventFilter(this);
        delete primary_.data();
        primary_.clear();
    }
}
//----------------------------------------
QQuickView* KtLockScreenManager::active_lock_view() const {
    QWindow* activeWindow = QGuiApplication::focusWindow();
    if (activeWindow == primary_.data())
        return primary_.data();

    for (auto* secondary : secondaries_) {
        if (activeWindow == secondary)
            return secondary;
    }

    return nullptr;
}
//----------------------------------------
void KtLockScreenManager::apply_primary_geometry() {
    auto* view = primary_view();
    if (!view)
        return;

    const auto screens = QGuiApplication::screens();
    if (primaryScreenId_ < 0 || primaryScreenId_ >= screens.size()) {
        view->hide();
        return;
    }

    QScreen* targetScreen = screens.at(primaryScreenId_);
    view->setScreen(targetScreen);

    const QRect screenRect = targetScreen->geometry();
    if (debugMode_) {
        const int width = qMax(320, static_cast<int>(screenRect.width() * debugSizeFraction_));
        const int height = qMax(240, static_cast<int>(screenRect.height() * debugSizeFraction_));
        view->setGeometry(screenRect.x(), screenRect.y(), width, height);
        view->show();
    }
    else {
        view->setGeometry(screenRect);
        view->showFullScreen();
    }

    if (auto* root = view->rootObject()) {
        root->setProperty("debugControlsVisible", debugMode_);
        root->setProperty("debugSizeText", debug_size_label(debugSizeFraction_));
    }

    for (auto* secondary : secondaries_)
        apply_secondary_geometry(secondary);
}
//----------------------------------------
void KtLockScreenManager::apply_secondary_geometry(QQuickView* view) {
    if (!view)
        return;

    const auto screens = QGuiApplication::screens();
    const int screenId = view->property("screenId").toInt();
    if (screenId < 0 || screenId >= screens.size()) {
        view->hide();
        return;
    }

    QScreen* targetScreen = screens.at(screenId);
    view->setScreen(targetScreen);

    if (debugMode_ && primary_view()) {
        const QRect anchor = primary_view()->geometry();
        const QRect screenRect = targetScreen->availableGeometry();
        const int width = qMin(640, qMax(320, screenRect.width() - anchor.width() - 24));
        const int height = qMin(480, qMax(240, screenRect.height() - 24));
        QRect debugRect(anchor.right() + 12, anchor.top(), width, height);

        if (debugRect.right() > screenRect.right())
            debugRect.moveLeft(qMin(screenRect.right() - debugRect.width() + 1, anchor.left()));
        if (debugRect.bottom() > screenRect.bottom())
            debugRect.moveTop(qMin(screenRect.bottom() - debugRect.height() + 1,
                                   anchor.bottom() + 12));
        if (debugRect.left() < screenRect.left())
            debugRect.moveLeft(screenRect.left());
        if (debugRect.top() < screenRect.top())
            debugRect.moveTop(screenRect.top());

        view->setGeometry(debugRect);
        view->show();
    }
    else {
        const QRect screenRect = targetScreen->geometry();
        view->setGeometry(screenRect);
        view->showFullScreen();
    }
}
//----------------------------------------
QQuickView* KtLockScreenManager::create_lock_view(const QString& typeName, int screenId) {
    auto* view = new QQuickView;
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    view->setColor(Qt::black);
    view->setFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    view->setProperty("screenId", screenId);
    view->installEventFilter(this);

    const QString appDir = QCoreApplication::applicationDirPath();
    view->engine()->addImportPath(appDir + QStringLiteral("/qml"));
    view->engine()->addImportPath(appDir);
    view->engine()->addImportPath(QStringLiteral("qrc:/qt/qml"));

    view->loadFromModule(QStringLiteral("KtAlarmClockQml"), typeName);

    if (view->status() == QQuickView::Error) {
        const auto errors = view->errors();
        for (const QQmlError& error : errors)
            qWarning() << "[LockScreen] QML error:" << error;
    }

    if (auto* root = view->rootObject())
        root->setProperty("screenId", screenId);

    return view;
}
//----------------------------------------
bool KtLockScreenManager::eventFilter(QObject* watched, QEvent* event) {
    if (!is_lock_view(watched))
        return QObject::eventFilter(watched, event);

    if (event->type() == QEvent::Close) {
        auto* closeEvent = static_cast<QCloseEvent*>(event);
        if (!canClose_) {
            closeEvent->ignore();
            return true;
        }

        if (visible_) {
            closeEvent->ignore();
            hide();
            emit clock_out(KtAlarmClock::WorkBreak);
            return true;
        }
    }

    if (event->type() == QEvent::WindowDeactivate && visible_) {
        QTimer::singleShot(0, this, [this]() {
            raise_all(false);
        });
    }

    return QObject::eventFilter(watched, event);
}
//----------------------------------------
QQuickView* KtLockScreenManager::find_secondary(int screenId) const {
    for (auto* view : secondaries_) {
        if (view && view->property("screenId").toInt() == screenId)
            return view;
    }
    return nullptr;
}
//----------------------------------------
QString KtLockScreenManager::format_force_time() const {
    return counterForce_ > 0 ? format_clock(counterForce_) : QString();
}
//----------------------------------------
void KtLockScreenManager::hide() {
    tickTimer_->stop();
    watchdogTimer_->stop();
    audioMuteGuard_.disengage();
    keyboardBlocker_.set_enabled(false);

    for (auto* overlay : secondaries_) {
        if (!overlay)
            continue;
        overlay->removeEventFilter(this);
        overlay->hide();
        overlay->deleteLater();
    }
    secondaries_.clear();

    if (primary_) {
        primary_->hide();
        if (auto* root = primary_->rootObject()) {
            root->setProperty("debugControlsVisible", false);
            QMetaObject::invokeMethod(root, "prepareForShow", Qt::DirectConnection);
        }
    }

    breakClock_.pause();
    isForced_ = false;
    canClose_ = false;
    counterForce_ = 0;
    forceEndMs_ = 0;
    unlockPending_ = false;
    debugSizeFraction_ = 1.0;
    unlockFailureSerial_ = 0;

    if (visible_) {
        visible_ = false;
        emit visible_changed();
    }
}
//----------------------------------------
void KtLockScreenManager::initial_formula() {
    const int operandA = QRandomGenerator::global()->bounded(1000);
    const int operandB = QRandomGenerator::global()->bounded(1000);
    formulaValue_ = operandA + operandB;
    formulaText_ = QStringLiteral("%1 + %2 =").arg(operandA).arg(operandB);
}
//----------------------------------------
bool KtLockScreenManager::is_lock_view(const QObject* object) const {
    if (!object)
        return false;

    if (object == primary_.data())
        return true;

    for (auto* view : secondaries_) {
        if (object == view)
            return true;
    }

    return false;
}
//----------------------------------------
void KtLockScreenManager::on_answer_submitted(const QString& answer, bool requireFormula) {
    try_unlock(answer, requireFormula);
}
//----------------------------------------
void KtLockScreenManager::on_application_state_changed(Qt::ApplicationState state) {
    if (!visible_)
        return;

    if (state == Qt::ApplicationActive) {
        sync_clocks_from_monotonic_clock();
        reconcile_screens();
        raise_all(true);
    }
}
//----------------------------------------
void KtLockScreenManager::on_debug_exit_requested() {
    if (!debugMode_ || !visible_)
        return;

    hide();
    emit clock_out(KtAlarmClock::WorkBreak);
}
//----------------------------------------
void KtLockScreenManager::on_debug_size_toggle_requested() {
    if (!debugMode_ || !primary_view())
        return;

    if (debugSizeFraction_ < 0.5)
        debugSizeFraction_ = 0.5;
    else if (debugSizeFraction_ < 1.0)
        debugSizeFraction_ = 1.0;
    else
        debugSizeFraction_ = 0.5;

    apply_primary_geometry();
    refresh_ui();
    raise_all(true);
}
//----------------------------------------
void KtLockScreenManager::on_secondary_unlock_requested() {
    try_unlock(QString(), false);
}
//----------------------------------------
void KtLockScreenManager::on_tick() {
    audioMuteGuard_.ensure_muted();
    sync_clocks_from_monotonic_clock();
}
//----------------------------------------
void KtLockScreenManager::on_watchdog() {
    audioMuteGuard_.ensure_muted();
    reconcile_screens();
}
//----------------------------------------
void KtLockScreenManager::prepare_primary_for_show() {
    if (!primary_)
        return;

    if (auto* root = primary_->rootObject())
        QMetaObject::invokeMethod(root, "prepareForShow", Qt::DirectConnection);
}
//----------------------------------------
QQuickView* KtLockScreenManager::primary_view() const {
    return primary_.data();
}
//----------------------------------------
void KtLockScreenManager::raise_all(bool requestFocus) {
    if (!visible_)
        return;

    QQuickView* focusTarget = requestFocus ? active_lock_view() : nullptr;
    if (!focusTarget)
        focusTarget = primary_view();

    if (primary_view())
        primary_view()->raise();

    for (auto* secondary : secondaries_) {
        if (secondary)
            secondary->raise();
    }

    if (requestFocus && focusTarget)
        focusTarget->requestActivate();
}
//----------------------------------------
void KtLockScreenManager::reconcile_screens() {
    if (!visible_ || !primary_view())
        return;

    const auto screens = QGuiApplication::screens();
    if (screens.isEmpty())
        return;

    QScreen* primaryScreen = QGuiApplication::primaryScreen();
    int newPrimaryId = primaryScreen ? screens.indexOf(primaryScreen) : 0;
    if (newPrimaryId < 0 || newPrimaryId >= screens.size())
        newPrimaryId = 0;

    if (primaryScreenId_ != newPrimaryId) {
        qDebug() << "[LockScreen] primary screen id" << primaryScreenId_ << "->" << newPrimaryId;
        primaryScreenId_ = newPrimaryId;
    }

    primary_view()->setProperty("screenId", primaryScreenId_);
    if (auto* root = primary_view()->rootObject())
        root->setProperty("screenId", primaryScreenId_);

    QVector<int> neededScreenIds;
    if (debugMode_) {
        neededScreenIds.append(primaryScreenId_);
    }
    else {
        for (int screenIndex = 0; screenIndex < screens.size(); ++screenIndex) {
            if (screenIndex != primaryScreenId_)
                neededScreenIds.append(screenIndex);
        }
    }

    for (int index = secondaries_.size() - 1; index >= 0; --index) {
        auto* view = secondaries_.at(index);
        if (!view) {
            secondaries_.removeAt(index);
            continue;
        }

        const int screenId = view->property("screenId").toInt();
        if (screenId < 0 || screenId >= screens.size() || !neededScreenIds.contains(screenId)) {
            view->removeEventFilter(this);
            view->hide();
            view->deleteLater();
            secondaries_.removeAt(index);
        }
    }

    for (int index = secondaries_.size() - 1; index >= 0; --index) {
        const int screenId = secondaries_.at(index)->property("screenId").toInt();
        for (int prior = 0; prior < index; ++prior) {
            if (secondaries_.at(prior)->property("screenId").toInt() == screenId) {
                auto* duplicate = secondaries_.at(index);
                duplicate->removeEventFilter(this);
                duplicate->hide();
                duplicate->deleteLater();
                secondaries_.removeAt(index);
                break;
            }
        }
    }

    for (int screenId : neededScreenIds) {
        if (find_secondary(screenId))
            continue;

        auto* overlay = create_lock_view(QStringLiteral("LockPrimary"), screenId);
        if (auto* root = overlay->rootObject()) {
            QObject::connect(root, SIGNAL(answerSubmitted(QString,bool)), this,
                             SLOT(on_answer_submitted(QString,bool)), Qt::UniqueConnection);
        }
        secondaries_.append(overlay);
    }

    refresh_ui();
    apply_primary_geometry();
    raise_all(true);

    qDebug() << "[LockScreen] reconcile screens=" << screens.size()
             << "primaryId=" << primaryScreenId_ << "secondaryCount=" << secondaries_.size();
}
//----------------------------------------
void KtLockScreenManager::refresh_ui() {
    const bool showUnlock = !isForced_;
    const bool showFormula = breakClock_.get_running();

    if (auto* view = primary_view()) {
        if (auto* root = view->rootObject()) {
            const bool keepFormulaVisible = root->property("formulaEntryActive").toBool();
            root->setProperty("debugControlsVisible", debugMode_);
            root->setProperty("debugSizeText", debug_size_label(debugSizeFraction_));
            root->setProperty("forced", isForced_);
            root->setProperty("forceText", format_force_time());
            root->setProperty("formulaText", formulaText_);
            root->setProperty("formulaVisible", showFormula || keepFormulaVisible);
            root->setProperty("unlockFailureSerial", unlockFailureSerial_);
            root->setProperty("unlockVisible", showUnlock);
        }
    }

    for (auto* secondary : secondaries_) {
        if (!secondary)
            continue;
        if (auto* root = secondary->rootObject()) {
            const bool keepFormulaVisible = root->property("formulaEntryActive").toBool();
            root->setProperty("debugControlsVisible", false);
            root->setProperty("debugSizeText", QString());
            root->setProperty("forced", isForced_);
            root->setProperty("forceText", format_force_time());
            root->setProperty("formulaText", formulaText_);
            root->setProperty("formulaVisible", showFormula || keepFormulaVisible);
            root->setProperty("unlockFailureSerial", unlockFailureSerial_);
            root->setProperty("unlockVisible", showUnlock);
        }
    }
}
//----------------------------------------
void KtLockScreenManager::retranslate() {
    if (auto* view = primary_view()) {
        if (view->engine())
            view->engine()->retranslate();
    }

    for (auto* secondary : secondaries_) {
        if (secondary && secondary->engine())
            secondary->engine()->retranslate();
    }
}
//----------------------------------------
void KtLockScreenManager::set_exiting(bool exiting) {
    if (exiting_ == exiting)
        return;

    exiting_ = exiting;
    emit exiting_changed();
}
//----------------------------------------
void KtLockScreenManager::set_view_can_close(bool canClose) {
    canClose_ = canClose;
}
//----------------------------------------
void KtLockScreenManager::show(int breakSeconds, int forceSeconds, bool debugMode) {
    debugMode_ = debugMode;

    const auto screens = QGuiApplication::screens();
    QScreen* primaryScreen = QGuiApplication::primaryScreen();
    primaryScreenId_ = primaryScreen ? screens.indexOf(primaryScreen) : 0;
    if (primaryScreenId_ < 0 || primaryScreenId_ >= screens.size())
        primaryScreenId_ = 0;

    unlockPending_ = false;
    isForced_ = false;
    canClose_ = debugMode_;
    counterForce_ = 0;
    forceEndMs_ = 0;
    unlockFailureSerial_ = 0;
    debugSizeFraction_ = debugMode_ ? 0.25 : 1.0;
    audioMuteGuard_.disengage();
    keyboardBlocker_.set_enabled(false);

    if (!primary_) {
        primary_ = create_lock_view(QStringLiteral("LockPrimary"), primaryScreenId_);
        if (auto* root = primary_->rootObject()) {
            QObject::connect(root, SIGNAL(answerSubmitted(QString,bool)), this,
                             SLOT(on_answer_submitted(QString,bool)), Qt::UniqueConnection);
            QObject::connect(root, SIGNAL(debugExitRequested()), this,
                             SLOT(on_debug_exit_requested()), Qt::UniqueConnection);
            QObject::connect(root, SIGNAL(debugSizeToggleRequested()), this,
                             SLOT(on_debug_size_toggle_requested()), Qt::UniqueConnection);
        }
    }

    breakClock_.start(KtAlarmClock::WorkBreak, breakSeconds);
    if (forceSeconds > 0 && breakSeconds > 0) {
        counterForce_ = qMin(forceSeconds, breakSeconds);
        forceEndMs_ = KtQpcClock::now_ms() + counterForce_ * 1000LL;
        isForced_ = true;
    }

    initial_formula();
    prepare_primary_for_show();
    refresh_ui();

    const bool notifyVisible = !visible_;
    visible_ = true;

    reconcile_screens();
    sync_clocks_from_monotonic_clock();
    refresh_ui();
    raise_all(true);

    tickTimer_->start();
    watchdogTimer_->start();

    if (!debugMode_) {
        keyboardBlocker_.set_enabled(true);
        audioMuteGuard_.engage();
    }

    if (notifyVisible)
        emit visible_changed();
}
//----------------------------------------
void KtLockScreenManager::sync_clocks_from_monotonic_clock() {
    if (!visible_)
        return;

    const bool wasRunning = breakClock_.get_running();
    breakClock_.sync_from_monotonic_clock();

    if (wasRunning && breakClock_.get_counter() <= 0) {
        breakClock_.pause();
        isForced_ = false;
        counterForce_ = 0;
        forceEndMs_ = 0;
        set_view_can_close(true);
    }

    sync_force_from_monotonic_clock();
    refresh_ui();
}
//----------------------------------------
void KtLockScreenManager::sync_force_from_monotonic_clock() {
    if (forceEndMs_ <= 0) {
        isForced_ = false;
        return;
    }

    const int remainingSec =
        static_cast<int>(qCeil((forceEndMs_ - KtQpcClock::now_ms()) / 1000.0));
    if (remainingSec <= 0) {
        isForced_ = false;
        counterForce_ = 0;
        forceEndMs_ = 0;
    }
    else {
        isForced_ = true;
        counterForce_ = remainingSec;
    }
}
//----------------------------------------
void KtLockScreenManager::try_unlock(const QString& answer, bool requireFormula) {
    if (exiting_ || unlockPending_ || isForced_)
        return;

    bool canUnlock = false;
    if (!breakClock_.get_running()) {
        canUnlock = true;
    }
    else if (requireFormula) {
        bool parseOk = false;
        const int answerValue = answer.trimmed().toInt(&parseOk);
        canUnlock = parseOk && (answerValue == formulaValue_);
    }

    set_view_can_close(canUnlock);

    if (!canUnlock) {
        ++unlockFailureSerial_;
        refresh_ui();
        return;
    }

    unlockPending_ = true;
    refresh_ui();

    QTimer::singleShot(0, this, [this]() {
        if (!exiting_) {
            hide();
            emit clock_out(KtAlarmClock::WorkBreak);
        }
        unlockPending_ = false;
    });
}
