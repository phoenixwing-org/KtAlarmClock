/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockSettingWindow.h
 * @brief       设置对话框（KtAlarmClockSettingDlg.qml）；界面见 .ui，可用 Qt Designer 编辑
 */
#ifndef KtAlarmClockSettingWindow_H
#define KtAlarmClockSettingWindow_H

#include <QDialog>

#include "KtAlarmClockParam.h"

class QCloseEvent;
class QLabel;
class QSlider;
class QSpinBox;
class QWidget;

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

    /** @brief 在锚点窗口附近显示 */
    void show_near(const QWidget* anchor);

public slots:
    /** @brief 从参数刷新 UI */
    void update_dialog();

    /** @brief 从 UI 写回参数（以 spinBox 秒数为准） */
    void update_infos();

signals:
    /** @brief 底部控制按钮动作（KtAlarmClock::ActionID） */
    void action_triggered(int actionId);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    /** @brief 同步滑条、输入框与时间标签，不触发互相连锁信号 */
    void apply_seconds_value(QSlider* slider, QSpinBox* spinBox, QLabel* timeLabel, int seconds);

    /**
     * @brief 绑定滑条与输入框双向同步（秒）
     * @param slider 滑条
     * @param spinBox 秒数输入框（对外数据源）
     * @param timeLabel 输入框右侧 m:ss 标签；可为 nullptr
     */
    void bind_spin_slider(QSlider* slider, QSpinBox* spinBox, QLabel* timeLabel);

    /** @brief 连接 .ui 控件信号 */
    void connect_ui_signals();

    /** @brief 格式化秒数为 m:ss */
    static QString format_slider_value(int seconds);

    /** @brief 恢复工作/休息/强制时长为默认值并写回 parameter */
    void restore_defaults();

private:
    Ui::KtAlarmClockSettingWindow* ui;           ///< 1. Designer 生成的界面
    KtAlarmClockParamShared        parameter_;   ///< 2. 共享参数
    bool                           debugLocate_; ///< 3. 调试日志
    bool                           syncingUi_;   ///< 4. 防止 spin/slider 互相同步时递归
};

#endif // KtAlarmClockSettingWindow_H
