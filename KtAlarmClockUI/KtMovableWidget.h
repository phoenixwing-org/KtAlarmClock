/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtMovableWidget.h
 * @brief       透明可拖动浮层控件
 */
#ifndef KtMovableWidget_H
#define KtMovableWidget_H

#include <QPoint>
#include <QWidget>

class QMouseEvent;

/**
 * @brief 透明浮层，左键拖动；位置限制在父控件客户区（可设边距）
 *
 * 用法：作为容器直接 add 子控件，或继承后扩展。
 */
class KtMovableWidget : public QWidget {
    Q_OBJECT

public:
    explicit KtMovableWidget(QWidget* parent = nullptr);
    ~KtMovableWidget() override; // @nodoc

public:
    /** @brief 父区域内限制拖动时的边距（像素） */
    int get_clamp_margins() const {
        return clampMargins_;
    }

    /** @brief 设置限制边距 */
    void set_clamp_margins(int margins);

    /** @brief 将当前位置限制在父控件可用区域内 */
    void clamp_to_parent_bounds();

signals:
    /** @brief 拖动导致位置变化后发出 */
    void position_changed(const QPoint& pos);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    /** @brief 按全局左上角移动并限制在父区域内 */
    void apply_clamped_position(const QPoint& globalTopLeft);

    /** @brief 父控件上允许放置的矩形（客户区减边距） */
    QRect parent_bounds() const;

private:
    int    clampMargins_; ///< 1. 限制边距
    bool   dragging_;     ///< 2. 左键拖动中
    QPoint dragOffset_;   ///< 3. 按下点相对控件左上角的偏移
};

#endif // KtMovableWidget_H
