/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtMainClockWidget.h
 * @brief       主浮窗胶囊计时器（MyClock.qml + KtAlarmClockDlg.qml Window）
 */
#ifndef KtMainClockWidget_H
#define KtMainClockWidget_H

#include "KtWallClockEngine.h"

#include <QPoint>
#include <QWidget>

class QLabel;
class QScreen;
class QShowEvent;
class QTimer;

/**
 * @brief 无边框置顶胶囊计时浮窗
 */
class KtMainClockWidget : public QWidget {
    Q_OBJECT

public:
    explicit KtMainClockWidget(QWidget* parent = nullptr);
    ~KtMainClockWidget() override;

public:
    /** @brief 暂停倒计时 */
    void clock_pause();

    /**
     * @brief 启动或恢复倒计时
     * @param leanFirstSecond 仅播放/继续续计时为 true，Next 与 ±60s 为 false
     */
    void clock_start(int state, int counterSec, bool leanFirstSecond = false);

    /** @brief 是否允许关闭窗口 */
    bool get_can_close() const {
        return canClose_;
    }

    /** @brief 剩余秒数（未启动时为 -100） */
    int get_counter() const;

    /** @brief 是否正在倒计时 */
    bool get_running() const;

    /** @brief 系统唤醒回到前台 */
    void handle_application_active();

    /** @brief 系统休眠（Qt::ApplicationSuspended） */
    void handle_application_suspended();

    /** @brief 设置是否允许关闭 */
    void set_can_close(bool canClose) {
        canClose_ = canClose;
    }

    /** @brief 从墙钟同步剩余秒数 */
    void sync_from_wall_clock();

protected:
    void closeEvent(QCloseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    /** @brief 移动窗口并限制在四边可用区域内（跨屏时切换 screen 以匹配 DPI） */
    void apply_clamped_position(const QPoint& globalTopLeft);

    /** @brief 将左上角限制在目标屏 availableGeometry 内（左/上/右/下四边） */
    QPoint clamp_to_screen(const QPoint& globalTopLeft) const;

    /** @brief 根据两次 tick 间隔推断休眠，工作时冻结在末次 tick */
    void detect_sleep_gap();

    /** @brief 校正当前位置，防止跨屏或 resize 后越界 */
    void ensure_within_screen();

    /** @brief 启动 1s tick 并立即同步一次显示 */
    void ensure_work_tick_running();

    /** @brief 格式化剩余秒数为 m:ss */
    static QString format_time(int counterSec);

    /** @brief 刷新时间文本与窗口尺寸 */
    void refresh_display();

    /** @brief 根据窗口矩形中心选取所在屏幕 */
    QScreen* screen_for_position(const QPoint& globalTopLeft) const;

    /** @brief 休眠冻结后续计、墙钟纠偏、补启 tick（多路径唤醒共用） */
    void try_resume_after_wake();

signals:
    /** @brief 阶段倒计时结束 */
    void clock_out(int state);

    /** @brief 请求在全局坐标弹出右键菜单 */
    void context_menu_requested(const QPoint& globalPos);

private slots:
    void on_tick();
    void on_wake_watchdog();

private:
    KtWallClockEngine workClock_;      ///< 1. 工作墙钟
    QLabel*           TimeLabel;     ///< 2. 时间文本
    QTimer*           TickTimer;     ///< 3. 1s 工作倒计时 tick
    QTimer*           WakeWatchTimer; ///< 10. 唤醒看门狗（休眠后进程恢复时补续计）
    bool              canClose_;   ///< 4. 是否允许关闭
    bool              dragging_;       ///< 5. 是否正在拖动
    QPoint            dragOffset_;     ///< 6. 按下时窗口角与鼠标的全局偏移
    int               initialCounter_; ///< 7. 未启动标记（-100）
    bool              clampingMove_;   ///< 8. 防止 clamp 递归触发 move
    qint64            lastTickMs_;     ///< 9. 上次 tick 墙钟 ms，用于休眠间隔检测
};

#endif // KtMainClockWidget_H
