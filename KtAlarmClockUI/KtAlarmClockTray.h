/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     Apache-2.0
 * @author      Kuntai
 * @file        KtAlarmClockTray.h
 * @brief       系统托盘
 */
#ifndef KtAlarmClockTray_H
#define KtAlarmClockTray_H

#include <QObject>
#include <QSystemTrayIcon>

class QMenu;

/**
 * @brief 系统托盘图标与上下文菜单
 */
class KtAlarmClockTray : public QObject {
    Q_OBJECT

public:
    explicit KtAlarmClockTray(QObject* parent = nullptr);
    ~KtAlarmClockTray() override;

public:
    /** @brief 隐藏托盘图标 */
    void hide();

    /** @brief 是否输出调试日志 */
    void set_debug_locate(bool enabled) {
        debugLocate_ = enabled;
    }

    /** @brief 休息中是否禁用菜单（对应 checkForbidden） */
    void set_forbidden(bool forbidden) {
        forbidden_ = forbidden;
    }

    /** @brief 托盘气泡提示 */
    void show_message(const QString& title, const QString& message, int durationMs = 3000);

private:
    /** @brief 构建托盘菜单 */
    void build_menu();

    /** @brief 休息中则忽略操作 */
    bool check_forbidden() const;

signals:
    /** @brief 菜单动作（KtAlarmClock::ActionID） */
    void action_triggered(int actionId);

private slots:
    void on_tray_activated(QSystemTrayIcon::ActivationReason reason);

private:
    QSystemTrayIcon* TrayIcon;   ///< 1. 托盘图标
    QMenu*           Menu;       ///< 2. 上下文菜单
    bool             debugLocate_; ///< 3. 调试日志
    bool             forbidden_;   ///< 4. 休息中禁用
};

#endif // KtAlarmClockTray_H
