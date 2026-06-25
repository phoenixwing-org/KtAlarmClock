/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtMovableWidget.cpp
 */
#include "KtMovableWidget.h"

#include <QMouseEvent>
#include <QRect>

//------------------------------------------------------
KtMovableWidget::KtMovableWidget(QWidget* parent)
    : QWidget(parent)
    , clampMargins_(0) // 1
    , dragging_(false) // 2
    , dragOffset_() {  // 3
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(false);
}
//------------------------------------------------------
KtMovableWidget::~KtMovableWidget() {
    // clampMargins_ // 1
    // dragging_     // 2
    // dragOffset_   // 3
}
//------------------------------------------------------
void KtMovableWidget::apply_clamped_position(const QPoint& globalTopLeft) {
    QWidget* host = parentWidget();
    if (!host)
        return;

    QPoint topLeft = host->mapFromGlobal(globalTopLeft);
    const QRect bounds = parent_bounds();

    if (topLeft.x() < bounds.left())
        topLeft.setX(bounds.left());
    if (topLeft.y() < bounds.top())
        topLeft.setY(bounds.top());
    if (topLeft.x() + width() > bounds.right() + 1)
        topLeft.setX(bounds.right() + 1 - width());
    if (topLeft.y() + height() > bounds.bottom() + 1)
        topLeft.setY(bounds.bottom() + 1 - height());

    if (width() > bounds.width())
        topLeft.setX(bounds.left());
    if (height() > bounds.height())
        topLeft.setY(bounds.top());

    if (pos() != topLeft) {
        move(topLeft);
        emit position_changed(topLeft);
    }
}
//------------------------------------------------------
void KtMovableWidget::clamp_to_parent_bounds() {
    apply_clamped_position(mapToGlobal(QPoint(0, 0)));
}
//------------------------------------------------------
void KtMovableWidget::mouseMoveEvent(QMouseEvent* event) {
    if (dragging_ && (event->buttons() & Qt::LeftButton)) {
        apply_clamped_position(event->globalPos() - dragOffset_);
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}
//------------------------------------------------------
void KtMovableWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        dragging_   = true;
        dragOffset_ = event->globalPos() - frameGeometry().topLeft();
        raise();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}
//------------------------------------------------------
void KtMovableWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        dragging_ = false;
        clamp_to_parent_bounds();
    }
    QWidget::mouseReleaseEvent(event);
}
//------------------------------------------------------
QRect KtMovableWidget::parent_bounds() const {
    QWidget* host = parentWidget();
    if (!host)
        return QRect();

    return host->rect().adjusted(clampMargins_, clampMargins_, -clampMargins_, -clampMargins_);
}
//------------------------------------------------------
void KtMovableWidget::set_clamp_margins(int margins) {
    clampMargins_ = qMax(0, margins);
}
