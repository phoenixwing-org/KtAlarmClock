/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     Apache-2.0
 * @author      Kuntai
 * @file        KtAlarmClockController.h
 * @brief       主流程控制器
 */
#ifndef KtAlarmClockController_H
#define KtAlarmClockController_H

#include <QObject>
#include <QPointer>
#include <QPoint>

#include "KtAlarmClockParam.h"

class KtAlarmClockCmd;
class KtAlarmClockSettingWindow;
class KtAlarmClockTray;
class KtLockScreenManager;
class KtMainClockWidget;
class QMenu;

/**
 * @brief 协调主浮窗、托盘、锁屏、设置与命令
 */
class KtAlarmClockController : public QObject {
    Q_OBJECT

public:
    explicit KtAlarmClockController(KtAlarmClockParamShared param, KtAlarmClockCmd* cmd,
                                    QObject* parent = nullptr);
    ~KtAlarmClockController() override;

public:
    /** @brief 主浮窗 */
    KtMainClockWidget* get_main_clock() const {
        return mainClock_;
    }

    /** @brief 是否输出调试日志 */
    void set_debug_locate(bool enabled);

    /** @brief 初始化 UI 与信号连接 */
    void start();

private:
    void apply_debug_defaults();

    /**
     * @brief 切换工作阶段并启动/暂停 UI
     * @param resetDurationFromParam 为 true 时工作阶段强制使用 parameter->WorkTime（Next / 休息结束）
     */
    void clock_start(int workStep, bool resetDurationFromParam = false);
    void clock_timeout_impl(int state);
    void close_all_windows();

    /** @brief 关闭并销毁设置窗 */
    void close_setting_dlg();

    void force_unload_over_dlg();

    /** @brief 休息中是否禁止用户操作 */
    bool is_forbidden() const;
    void load_over_dlg();

    /** @brief 设置窗打开时，从 parameter 刷新界面与播放按钮 */
    void refresh_setting_ui();

    /** @brief 执行动作（内部状态机，不经 forbidden 校验） */
    void run_command(int actionId);

    void show_pop_menu(const QPoint& globalPos);

    /** @brief 显示或复用唯一设置窗（跟随 globalPos 所在屏幕） */
    void show_setting_dlg(const QPoint& globalPos);

    void sync_param_from_setting();

public slots:
    /** @brief 用户操作统一入口（托盘/右键/设置底部按钮） */
    void dispatch_user_action(int actionId);

    /** @brief 二次启动时由单实例守卫触发：显示主浮窗或托盘提示 */
    void on_second_instance_activate();

private slots:
    void on_clock_timeout(int state);
    void on_context_menu_requested(const QPoint& globalPos);

    /** @brief 设置窗底部按钮：先写回参数再分发动作 */
    void on_setting_action(int actionId);

private:
    KtAlarmClockParamShared    parameter;    ///< 1. 参数
    KtAlarmClockCmd*           cmd_;         ///< 2. 命令（退出等）
    KtMainClockWidget*         mainClock_;   ///< 3. 主浮窗
    KtAlarmClockTray*          tray_;        ///< 4. 托盘
    KtLockScreenManager*       lockScreen_;  ///< 5. 锁屏
    QPointer<KtAlarmClockSettingWindow> settingDlg_; ///< 6. 唯一设置窗（QPointer 自动跟踪销毁）
    QMenu*                     popMenu_;     ///< 7. 右键菜单
    bool                       loop_;        ///< 8. 是否循环工作/休息
    int                        workStep_;    ///< 9. 当前阶段
    bool                       debugLocate_; ///< 10. 调试日志
    static constexpr int       kInitialCounter = -100;
};

#endif // KtAlarmClockController_H
