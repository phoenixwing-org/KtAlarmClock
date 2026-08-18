/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     Apache-2.0
 * @author      Kuntai
 * @file        KtLockScreenSecondaryWidget.h
 * @brief       副屏锁屏遮罩
 */
#ifndef KtLockScreenSecondaryWidget_H
#define KtLockScreenSecondaryWidget_H

#include <QWidget>

class QCloseEvent;
class QLabel;
class QPushButton;
class QResizeEvent;

/**
 * @brief 副显示器上的锁屏遮罩
 */
class KtLockScreenSecondaryWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief 构造副屏遮罩
     * @param screenId 目标屏幕索引
     * @param parent 父窗口；通常为 nullptr
     */
    explicit KtLockScreenSecondaryWidget(int screenId, QWidget* parent = nullptr);
    ~KtLockScreenSecondaryWidget(); // @nodoc

public:
    /**
     * @brief 按屏幕或调试布局显示
     * @param fullScreen 生产环境全屏
     * @param debugMode 单屏时在 anchor 旁开小窗
     * @param anchor 调试时参照的主屏 widget
     */
    void apply_screen_geometry(bool fullScreen, bool debugMode, const QWidget* anchor);

    /** @brief 置顶但不 activateWindow */
    void raise_quiet();

    /** @brief 设置是否允许关闭窗口 */
    void set_can_close(bool canClose);

    /** @brief 切换强制等待态；副屏文案始终显示，仅 Unlock 在强制期隐藏 */
    void set_forced(bool forced, int counterForce);

    /** @brief 目标屏幕索引（热插拔后由 Manager 更新） */
    void set_screen_id(int screenId) {
        screenId_ = screenId;
    }

private:
    /** @brief 构建 UI */
    void build_ui();

    /** @brief 按屏宽设置文案区，避免 WordWrap 被裁切 */
    void update_message_layout();

protected:
    /** @brief 关闭事件 */
    void closeEvent(QCloseEvent* event) override;

    /** @brief 全屏尺寸变化时更新文案换行宽度 */
    void resizeEvent(QResizeEvent* event) override;

signals:
    /** @brief 解锁请求 */
    void unlock_requested();

private:
    int          screenId_;    ///< 1. 目标屏幕索引
    bool         canClose_;    ///< 2. 是否允许关闭
    QLabel*      LabelMessage; ///< 3. 休息文案
    QLabel*      LabelForce;   ///< 4. 强制倒计时
    QPushButton* BtnUnlock;    ///< 5. 解锁按钮
};

#endif // KtLockScreenSecondaryWidget_H
