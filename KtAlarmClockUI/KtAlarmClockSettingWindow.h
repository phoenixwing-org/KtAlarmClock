/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     Apache-2.0
 * @author      Kuntai
 * @file        KtAlarmClockSettingWindow.h
 * @brief       设置对话框；界面见 .ui，可用 Qt Designer 编辑
 */
#ifndef KtAlarmClockSettingWindow_H
#define KtAlarmClockSettingWindow_H

#include <QDialog>

#include "KtAlarmClockParam.h"

class KtDurationEdit;
class QCloseEvent;
class QPoint;
class QSlider;

namespace Ui {
class KtAlarmClockSettingWindow;
}

/**
 * @brief 护眼闹钟设置对话框（标准 QDialog + Qt Designer）
 */
class KtAlarmClockSettingWindow : public QDialog {
    Q_OBJECT

public:
    explicit KtAlarmClockSettingWindow(QWidget* parent = nullptr);
    ~KtAlarmClockSettingWindow() override;

public:
    /** @brief 是否输出调试日志 */
    void set_debug_locate(bool enabled) {
        debugLocate_ = enabled;
    }

    /** @brief 绑定共享参数对象 */
    void set_param(KtAlarmClockParamShared param);

    /** @brief 在全局参考点（通常为鼠标）附近显示，并限制在当前屏幕内 */
    void show_near(const QPoint& globalRef);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    /** @brief 高 DPI 下按字体度量列宽与行高，避免标签叠字、按钮撑爆 */
    void apply_layout_metrics();

    /** @brief 同步滑条与 m:ss 输入框，不触发互相连锁信号 */
    void apply_seconds_value(QSlider* slider, KtDurationEdit* durationEdit, int seconds);

    /**
     * @brief 绑定滑条与 m:ss 输入框双向同步（秒）
     * @param slider 滑条
     * @param durationEdit 时长输入框（对外数据源）
     */
    void bind_duration_slider(QSlider* slider, KtDurationEdit* durationEdit);

    /** @brief 连接 .ui 控件信号 */
    void connect_ui_signals();

    /** @brief 恢复工作/休息/强制时长为默认值并写回 parameter */
    void restore_defaults();

signals:
    /** @brief 底部控制按钮动作（KtAlarmClock::ActionID） */
    void action_triggered(int actionId);

public slots:
    /** @brief 从参数刷新 UI */
    void update_dialog();

    /** @brief 从 UI 写回参数（以 m:ss 输入框秒数为准） */
    void update_infos();

    /** @brief 按计时状态切换播放/暂停图标与 tooltip */
    void update_play_pause_button(bool running);

private:
    Ui::KtAlarmClockSettingWindow* ui;           ///< 1. Designer 生成的界面
    KtAlarmClockParamShared        parameter_;   ///< 2. 共享参数
    bool                           debugLocate_; ///< 3. 调试日志
    bool                           syncingUi_;   ///< 4. 防止控件互相同步时递归
};

#endif // KtAlarmClockSettingWindow_H
