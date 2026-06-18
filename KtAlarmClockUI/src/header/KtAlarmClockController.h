#ifndef KtAlarmClockController_H
#define KtAlarmClockController_H

#include "KtAlarmClockParam.h"

#include <QObject>
#include <QString>
#include <QVariantMap>

class KtAlarmClockCmd;
class KtAlarmClockQmlShell;
class KtAlarmClockViewModel;
class KtClockRuntime;
class KtLockScreenManager;
class KtTranslationManager;
class KtTrayService;

class KtAlarmClockController : public QObject {
    Q_OBJECT

public:
    explicit KtAlarmClockController(KtAlarmClockParamShared param, KtAlarmClockCmd* cmd,
                                    KtTranslationManager* translationManager,
                                    QObject* parent = nullptr);
    ~KtAlarmClockController() override;

    void set_debug_locate(bool enabled);
    void start();
    KtAlarmClockViewModel* view_model() const {
        return model_;
    }

private:
    void clock_timeout_impl(int state);
    void close_all_windows();
    void dispatch_user_action_impl(int actionId);
    void enter_break_phase();
    void enter_idle_phase();
    void enter_phase(int phase, bool resetDurationFromParam = false);
    void enter_work_phase(bool resetDurationFromParam);
    void force_unload_over_dlg();
    void handle_play_pause_action();
    void handle_time_adjustment(int deltaSeconds);
    bool is_forbidden() const;
    void load_over_dlg();
    void run_command(int actionId);
    void set_phase(int phase);
    void sync_model();

public slots:
    void dispatch_user_action(int actionId);

    Q_INVOKABLE QVariantMap clampWindowPosition(int globalX, int globalY, int width,
                                                int height) const;
    Q_INVOKABLE QVariantMap clampWindowPositionOnCurrentScreen(
        int globalX, int globalY, int width, int height, int currentCenterX,
        int currentCenterY) const;
    Q_INVOKABLE QVariantMap initialWindowPosition(int width, int height) const;
    Q_INVOKABLE void adjustRemaining(int deltaSeconds);
    Q_INVOKABLE void closeSettings();
    Q_INVOKABLE void nextLoop();
    Q_INVOKABLE void openSettings();
    Q_INVOKABLE void playPause();
    Q_INVOKABLE void quit();
    Q_INVOKABLE void resetSettings();
    Q_INVOKABLE void saveSettings(int workSeconds, int breakSeconds, int forceSeconds);
    Q_INVOKABLE void saveSettingsAndStart(int workSeconds, int breakSeconds, int forceSeconds);
    Q_INVOKABLE void setLocale(const QString& localeName);
    Q_INVOKABLE void showContextMenu(int globalX, int globalY);
    Q_INVOKABLE void startBreak();

private slots:
    void on_clock_timeout(int state);
    void on_clock_updated(int state, int counterSec, bool running);

private:
    KtAlarmClockParamShared parameter;
    KtAlarmClockCmd* cmd_;
    KtTranslationManager* translationManager_;
    KtClockRuntime* runtime_;
    KtTrayService* tray_;
    KtLockScreenManager* lockScreen_;
    KtAlarmClockViewModel* model_;
    KtAlarmClockQmlShell* qmlShell_;
    bool loop_;
    int phase_;
    bool debugLocate_;
    bool lockDebugMode_;
    static constexpr int kInitialCounter = -100;
};

#endif // KtAlarmClockController_H
