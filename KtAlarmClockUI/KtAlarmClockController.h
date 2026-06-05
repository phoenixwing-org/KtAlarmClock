/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockController.h
 * @brief       主流程控制器
 */
#ifndef KtAlarmClockController_H
#define KtAlarmClockController_H

#include <QObject>
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

public slots:
    /** @brief 执行托盘/设置/菜单动作（原 onAction 信号） */
    void run_command(int actionId);

private slots:
    void on_clock_timeout(int state);
    void on_context_menu_requested(const QPoint& globalPos);

private:
    void apply_debug_defaults();
    /**
     * @brief 切换工作阶段并启动/暂停 UI
     * @param resetDurationFromParam 为 true 时工作阶段强制使用 parameter->WorkTime（Next / 休息结束）
     */
    void clock_start(int workStep, bool resetDurationFromParam = false);
    void clock_timeout_impl(int state);
    void close_all_windows();
    void force_unload_over_dlg();
    void load_over_dlg();
    void load_setting_dlg();
    void show_pop_menu(const QPoint& globalPos);
    void sync_param_from_setting();
    void unload_setting_dlg();

    /** @brief 休息中是否禁止托盘/菜单操作 */
    bool is_forbidden() const;

private:
    KtAlarmClockParamShared    parameter;    ///< 1. 参数
    KtAlarmClockCmd*           cmd_;         ///< 2. 命令（退出等）
    KtMainClockWidget*         mainClock_;   ///< 3. 主浮窗
    KtAlarmClockTray*          tray_;        ///< 4. 托盘
    KtLockScreenManager*       lockScreen_;  ///< 5. 锁屏
    KtAlarmClockSettingWindow* settingDlg_;  ///< 6. 设置窗
    QMenu*                     popMenu_;     ///< 7. 右键菜单
    bool                       loop_;        ///< 8. 是否循环工作/休息
    int                        workStep_;    ///< 9. 当前阶段
    bool                       debugLocate_; ///< 10. 调试日志
    static constexpr int       kInitialCounter = -100;
};

#endif // KtAlarmClockController_H
