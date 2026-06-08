/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtScreenUtil.cpp
 */

#include "KtScreenUtil.h"

#include <QGuiApplication>
#include <QScreen>
#include <QWidget>
#include <QWindow>

namespace KtScreenUtil {

namespace {

QPoint clamp_top_left(const QPoint& topLeft, const QSize& size, const QRect& available) {
    int posX = topLeft.x();
    int posY = topLeft.y();

    if (posX < available.left())
        posX = available.left();
    if (posY < available.top())
        posY = available.top();
    if (posX + size.width() > available.right() + 1)
        posX = available.right() + 1 - size.width();
    if (posY + size.height() > available.bottom() + 1)
        posY = available.bottom() + 1 - size.height();

    if (size.width() > available.width())
        posX = available.left();
    if (size.height() > available.height())
        posY = available.top();

    return QPoint(posX, posY);
}

} // namespace

//------------------------------------------------------
QScreen* screen_at(const QPoint& globalPos) {
    QScreen* screen = QGuiApplication::screenAt(globalPos);
    if (!screen)
        screen = QGuiApplication::primaryScreen();
    return screen;
}
//------------------------------------------------------
QPoint place_popup(const QPoint& globalRef, const QSize& popupSize, bool belowRef) {
    QScreen* screen = screen_at(globalRef);
    if (!screen || popupSize.isEmpty())
        return globalRef;

    const QRect available = screen->availableGeometry();
    QPoint      topLeft   = globalRef;

    if (belowRef)
        topLeft.setY(globalRef.y() + 8);
    else
        topLeft.setY(globalRef.y() - popupSize.height() - 8);

    if (topLeft.y() + popupSize.height() > available.bottom() + 1)
        topLeft.setY(globalRef.y() - popupSize.height() - 8);
    if (topLeft.y() < available.top())
        topLeft.setY(globalRef.y() + 8);

    if (topLeft.x() + popupSize.width() > available.right() + 1)
        topLeft.setX(globalRef.x() - popupSize.width());
    if (topLeft.x() < available.left())
        topLeft.setX(globalRef.x());

    return clamp_top_left(topLeft, popupSize, available);
}
//------------------------------------------------------
void assign_screen(QWidget* widget, const QPoint& globalRef) {
    if (!widget) return;

    QScreen* screen = screen_at(globalRef);
    if (!screen) return;

    if (auto* window = widget->windowHandle()) {
        if (window->screen() != screen)
            window->setScreen(screen);
    }
}

} // namespace KtScreenUtil
