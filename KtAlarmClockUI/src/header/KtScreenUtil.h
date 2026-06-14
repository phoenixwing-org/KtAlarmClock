#ifndef KtScreenUtil_H
#define KtScreenUtil_H

#include <QPoint>
#include <QSize>

class QScreen;

namespace KtScreenUtil {

QScreen* screen_at(const QPoint& globalPos);
QPoint place_popup(const QPoint& globalRef, const QSize& popupSize, bool belowRef = true);

} // namespace KtScreenUtil

#endif // KtScreenUtil_H
