/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtLockScreenManager.h
 * @brief       多屏锁屏总控
 */
#ifndef KtLockScreenManager_H
#define KtLockScreenManager_H

#include "KtWallClockEngine.h"

#include <QObject>
#include <QPointer>
#include <QVector>

class KtLockScreenPrimaryWidget;
class KtLockScreenSecondaryWidget;
class QTimer;

/**
 * @brief 多屏锁屏总控
 *
 * 管理主屏/副屏 QWidget 遮罩、休息墙钟、算式解锁与 QML 属性 visible/exiting。
 */
class KtLockScreenManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool visible READ get_visible NOTIFY visible_changed)
    Q_PROPERTY(bool exiting READ get_exiting WRITE set_exiting NOTIFY exiting_changed)

public:
    explicit KtLockScreenManager(QObject* parent = nullptr);
    ~KtLockScreenManager();

public:
    /** @brief 是否正在退出动画 */
    bool get_exiting() const {
        return exiting_;
    }

    /** @brief 锁屏是否显示 */
    bool get_visible() const {
        return visible_;
    }

    /** @brief 隐藏锁屏 */
    void hide();

    /** @brief 设置是否正在退出动画 */
    void set_exiting(bool exiting);

    /**
     * @brief 显示多屏锁屏
     * @param breakSeconds 休息倒计时（秒）
     * @param forceSeconds 强制等待（秒）；须小于 breakSeconds 才生效
     * @param debugMode 单屏调试布局
     */
    void show(int breakSeconds, int forceSeconds, bool debugMode);

signals:
    /** @brief 休息结束并解锁；参数为下一阶段 state */
    void clock_out(int state);

    /** @brief 退出动画状态改变 */
    void exiting_changed();

    /** @brief 锁屏显示状态改变 */
    void visible_changed();

private slots:

    /** @brief 应用状态改变 */
    void on_application_state_changed(Qt::ApplicationState state);

    /** @brief 定时器超时 */
    void on_tick();

    /** @brief 解锁请求 */
    void on_unlock_requested();

    /** @brief 多屏巡检 */
    void on_watchdog();

    /** @brief 调试：强制退出锁屏 */
    void on_debug_exit_requested();

    /** @brief 调试：切换半屏/全屏 */
    void on_debug_size_toggle_requested();

private:
    /** @brief 格式化休息时间 */
    QString format_break_time() const;

    /** @brief 初始化算式 */
    void initial_formula();

    /** @brief 获取主屏锁屏 */
    KtLockScreenPrimaryWidget* primary_widget() const;

    /**
     * @brief 置顶所有屏
     * @param requestFocus 是否请求焦点
     */
    void raise_all(bool requestFocus);

    /** @brief 重新协调屏幕 */
    void reconcile_screens();

    /** @brief 刷新 UI */
    void refresh_ui();

    /** @brief 同步强制墙钟 */
    void sync_force_from_wall_clock();

    /** @brief 同步墙钟 */
    void sync_wall_clocks();

    /**
     * @brief 尝试解锁
     * @param fromSecondary 是否从副屏解锁
     */
    void try_unlock(bool fromSecondary);

    /** @brief 更新算式可见性 */
    void update_formula_visibility();

    /** @brief 按当前 debug/生产 参数刷新主屏几何 */
    void apply_primary_geometry();

private:
    KtWallClockEngine                     breakClock_;        ///< 1. 休息墙钟
    QPointer<KtLockScreenPrimaryWidget>   primary_;           ///< 2. 主屏锁屏
    QTimer*                               tickTimer_;         ///< 3. 1s 同步定时器
    QTimer*                               watchdogTimer_;     ///< 4. 10s 多屏巡检
    bool                                  visible_;           ///< 5. 锁屏是否显示
    bool                                  exiting_;           ///< 6. 退出动画中
    bool                                  debugMode_;         ///< 7. 调试布局
    bool                                  unlockPending_;     ///< 8. 解锁流程进行中
    bool                                  isForced_;          ///< 9. 强制等待期
    int                                   counterForce_;      ///< 10. 强制剩余秒
    qint64                                forceEndMs_;        ///< 11. 强制结束墙钟(ms)
    int                                   formulaValue_;      ///< 12. 算式正确答案
    int                                   primaryScreenId_;   ///< 13. 主屏索引
    QVector<KtLockScreenSecondaryWidget*> secondaries_;       ///< 14. 副屏遮罩
    double                                debugSizeFraction_; ///< 15. debug 占屏比(0.25/0.5/1)
};

#endif // KtLockScreenManager_H
