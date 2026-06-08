/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtScreenUtil.h
 * @brief       弹出窗口/菜单相对鼠标定位并限制在当前屏幕内
 */
#ifndef KtScreenUtil_H
#define KtScreenUtil_H

#include <QPoint>
#include <QSize>

class QScreen;
class QWidget;

namespace KtScreenUtil {

/** @brief 取全局坐标所在屏幕，失败时回退主屏 */
QScreen* screen_at(const QPoint& globalPos);

/**
 * @brief 将弹出物左上角放在参考点附近，并限制在参考点所在屏的可用区域内
 * @param globalRef 通常为鼠标全局坐标
 * @param popupSize 弹出物尺寸（对话框 frame、菜单 sizeHint 等）
 * @param belowRef  true 时优先放在参考点下方
 */
QPoint place_popup(const QPoint& globalRef, const QSize& popupSize, bool belowRef = true);

/** @brief 为弹出物绑定目标屏幕（跨 DPI 屏时避免缩放错位） */
void assign_screen(QWidget* widget, const QPoint& globalRef);

} // namespace KtScreenUtil

#endif // KtScreenUtil_H
