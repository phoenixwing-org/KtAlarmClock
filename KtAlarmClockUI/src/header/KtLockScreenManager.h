#ifndef KtLockScreenManager_H
#define KtLockScreenManager_H

#include "KtAudioMuteGuard.h"
#include "KtKeyboardBlocker.h"
#include "KtMonotonicCountdown.h"

#include <QObject>
#include <QPointer>
#include <QString>
#include <QVector>

class QEvent;
class QQuickView;
class QTimer;

class KtLockScreenManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool visible READ get_visible NOTIFY visible_changed)
    Q_PROPERTY(bool exiting READ get_exiting WRITE set_exiting NOTIFY exiting_changed)

public:
    explicit KtLockScreenManager(QObject* parent = nullptr);
    ~KtLockScreenManager() override;

    bool get_exiting() const {
        return exiting_;
    }

    bool get_visible() const {
        return visible_;
    }

    void hide();
    void retranslate();
    void set_exiting(bool exiting);
    void show(int breakSeconds, int forceSeconds, bool debugMode);

signals:
    void clock_out(int state);
    void exiting_changed();
    void visible_changed();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void on_application_state_changed(Qt::ApplicationState state);
    void on_answer_submitted(const QString& answer, bool requireFormula);
    void on_debug_exit_requested();
    void on_debug_size_toggle_requested();
    void on_secondary_unlock_requested();
    void on_tick();
    void on_watchdog();

private:
    void apply_primary_geometry();
    void apply_secondary_geometry(QQuickView* view);
    QQuickView* active_lock_view() const;
    QQuickView* create_lock_view(const QString& typeName, int screenId);
    QQuickView* find_secondary(int screenId) const;
    QString format_force_time() const;
    void initial_formula();
    bool is_lock_view(const QObject* object) const;
    void prepare_primary_for_show();
    QQuickView* primary_view() const;
    void raise_all(bool requestFocus);
    void reconcile_screens();
    void refresh_ui();
    void set_view_can_close(bool canClose);
    void sync_clocks_from_monotonic_clock();
    void sync_force_from_monotonic_clock();
    void try_unlock(const QString& answer, bool requireFormula);

private:
    KtMonotonicCountdown breakClock_;
    KtAudioMuteGuard     audioMuteGuard_;
    KtKeyboardBlocker    keyboardBlocker_;
    QPointer<QQuickView> primary_;
    QTimer* tickTimer_;
    QTimer* watchdogTimer_;
    bool visible_;
    bool exiting_;
    bool debugMode_;
    bool unlockPending_;
    bool isForced_;
    bool canClose_;
    int counterForce_;
    qint64 forceEndMs_;
    int formulaValue_;
    QString formulaText_;
    int unlockFailureSerial_;
    int primaryScreenId_;
    QVector<QQuickView*> secondaries_;
    double debugSizeFraction_;
};

#endif // KtLockScreenManager_H
