/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtLockScreenPrimaryWidget.h
 * @brief       主屏锁屏 QWidget
 */
#ifndef KtLockScreenPrimaryWidget_H
#define KtLockScreenPrimaryWidget_H

// QT
#include <QWidget>

// pre-declare
class QCloseEvent;
class QEvent;
class QKeyEvent;
class QResizeEvent;
class QShowEvent;
class KtMovableWidget;
class QLabel;
class QLineEdit;
class QPushButton;

/**
 * @brief 主显示器上的全屏锁屏界面
 *
 * 布局：左上解锁区（算式 + 输入 + 按钮）；茶杯、KT 图标与休息倒计时为独立可拖动浮层。
 */
class KtLockScreenPrimaryWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief 构造主屏锁屏窗口
     * @param screenId QGuiApplication::screens() 中的屏幕索引
     * @param parent 父窗口；通常为 nullptr
     */
    explicit KtLockScreenPrimaryWidget(int screenId, QWidget* parent = nullptr);
    ~KtLockScreenPrimaryWidget(); // @nodoc
public:
    /** @brief 答案输入框文本（供 Manager 验算） */
    QString answer_text() const;

    /**
     * @brief 按屏幕几何显示
     * @param fullScreen 生产环境 true 时 showFullScreen()
     * @param debugMode 调试布局
     * @param debugFraction debug 下占屏比例（0.25 / 0.5 / 1.0）
     */
    void apply_screen_geometry(bool fullScreen, bool debugMode, double debugFraction = 1.0);

    /** @brief 显示/隐藏调试按钮栏 */
    void set_debug_controls_visible(bool visible);

    /** @brief 更新「半屏/全屏」按钮文案 */
    void update_debug_size_button_text(double debugFraction);

    /** @brief 清空答案输入 */
    void clear_answer();

    /** @brief 目标屏幕索引（热插拔后由 Manager 更新） */
    void set_screen_id(int screenId) {
        screenId_ = screenId;
    }

    /** @brief 置顶但不 activateWindow */
    void raise_quiet();

    /** @brief 唤醒后修正浮层位置并延迟聚焦答案框 */
    void refresh_after_wake();

    /** @brief 设置是否允许关闭窗口 */
    void set_can_close(bool canClose);

    /** @brief 更新休息倒计时标签 */
    void set_break_time_text(const QString& text);

    /** @brief 显示/隐藏强制等待秒数 */
    void set_force_visible(bool visible, int seconds);

    /** @brief 设置算式文本 */
    void set_formula_text(const QString& text);

    /** @brief 显示/隐藏算式行 */
    void set_formula_visible(bool visible);

    /** @brief 设置提示消息（如答错） */
    void set_message_text(const QString& text);

    /** @brief 显示/隐藏整块解锁面板 */
    void set_unlock_panel_visible(bool visible);

    /** @brief 提交答案（键盘/按钮共用） */
    void submit_answer();

signals:
    /** @brief 用户提交答案（键盘或 Unlock 按钮） */
    void answer_submitted();

    /** @brief 调试：请求退出锁屏 */
    void debug_exit_requested();

    /** @brief 调试：请求切换半屏/全屏 */
    void debug_size_toggle_requested();

protected:
    /** @brief 未到点或答错时禁止关闭 */
    void closeEvent(QCloseEvent* event) override;

    /** @brief 显示后尝试聚焦答案框 */
    void showEvent(QShowEvent* event) override;

    /** @brief 尺寸变化时将浮层限制在可视区域内 */
    void resizeEvent(QResizeEvent* event) override;

    /** @brief 失焦时静默置顶 */
    bool event(QEvent* event) override;

    /** @brief 回车提交；数字键写入答案框（无需先点输入框） */
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    /** @brief Unlock 按钮 → submit_answer */
    void on_unlock_clicked();

    /** @brief 调试退出按钮 */
    void on_debug_exit_clicked();

    /** @brief 调试半屏/全屏按钮 */
    void on_debug_size_clicked();

private:
    /** @brief 创建解锁区与可拖动浮层 */
    void build_ui();

    /** @brief 解锁区可见时将焦点置于答案输入框 */
    void focus_answer_input();

    /** @brief 向答案框追加一位数字（窗口级键盘捕获） */
    void append_answer_digit(QChar digit);

    /** @brief 更新休息倒计时浮层尺寸 */
    void sync_break_time_drag_size();

    /** @brief 将所有可拖动浮层限制在窗口边距内 */
    void clamp_floating_widgets();

    /** @brief 首次显示时将三个浮层置于默认位置 */
    void place_floating_widgets_default();

    /** @brief 将浮层置于最前 */
    void raise_floating_widgets();

private:
    int              screenId_;            ///< 1. 目标屏幕索引
    bool             canClose_;            ///< 2. 是否允许关闭
    QLabel*          LabelMsg;             ///< 3. 提示/答错消息
    QLabel*          LabelFormula;         ///< 4. 算式标签
    QLineEdit*       EditAnswer;           ///< 5. 答案输入
    QPushButton*     BtnUnlock;            ///< 6. 解锁按钮
    QWidget*         UnlockPanel;          ///< 7. 解锁区容器
    QWidget*         FormulaRow;           ///< 8. 算式行容器
    QLabel*          LabelBreakTime;       ///< 9. 休息倒计时
    QLabel*          LabelForce;           ///< 10. 强制等待秒数（主屏隐藏）
    QLabel*          ImgCoffee;            ///< 11. 茶杯图
    QWidget*         DebugPanel;           ///< 12. 调试按钮栏
    QPushButton*     BtnDebugExit;         ///< 13. 调试退出
    QPushButton*     BtnDebugSize;         ///< 14. 半屏/全屏切换
    KtMovableWidget* DragCoffee;           ///< 15. 茶杯浮层
    KtMovableWidget* DragKtBlue;           ///< 16. KT 图标浮层
    KtMovableWidget* DragBreakTime;        ///< 17. 倒计时浮层
    QLabel*          ImgKtBlue;            ///< 18. KT 图标
    bool             floatingWidgetsPlaced_; ///< 19. 是否已做过首次摆放
};

#endif // KtLockScreenPrimaryWidget_H
