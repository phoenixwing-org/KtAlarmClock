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
class QLabel;
class QLineEdit;
class QPushButton;

/**
 * @brief 主显示器上的全屏锁屏界面
 *
 * 布局：左上解锁区（算式 + 输入 + 按钮），中央茶杯图标与休息倒计时。
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

    /** @brief 失焦时静默置顶 */
    bool event(QEvent* event) override;

    /** @brief 空格/回车提交；数字输入交给 LineEdit */
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    /** @brief Unlock 按钮 → submit_answer */
    void on_unlock_clicked();

    /** @brief 调试退出按钮 */
    void on_debug_exit_clicked();

    /** @brief 调试半屏/全屏按钮 */
    void on_debug_size_clicked();

private:
    /** @brief 创建解锁区与中央倒计时布局 */
    void build_ui();

private:
    int          screenId_;      ///< 1. 目标屏幕索引
    bool         canClose_;      ///< 2. 是否允许关闭
    QLabel*      LabelMsg;       ///< 3. 提示/答错消息
    QLabel*      LabelFormula;   ///< 4. 算式标签
    QLineEdit*   EditAnswer;     ///< 5. 答案输入
    QPushButton* BtnUnlock;      ///< 6. 解锁按钮
    QWidget*     UnlockPanel;    ///< 7. 解锁区容器
    QWidget*     FormulaRow;     ///< 8. 算式行容器
    QLabel*      LabelBreakTime; ///< 9. 休息倒计时
    QLabel*      LabelForce;     ///< 10. 强制等待秒数
    QLabel*      ImgCoffee;      ///< 11. 茶杯图标
    QWidget*     DebugPanel;     ///< 12. 调试按钮栏
    QPushButton* BtnDebugExit;   ///< 13. 调试退出
    QPushButton* BtnDebugSize;   ///< 14. 半屏/全屏切换
};

#endif // KtLockScreenPrimaryWidget_H
