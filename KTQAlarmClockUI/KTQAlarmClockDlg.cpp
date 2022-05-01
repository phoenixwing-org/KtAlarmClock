/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockDlg.cpp
 */

#include "KTQAlarmClockDlg.h"

#define ForTestTimter 0
#if ForTestTimter
#define ShowSubDialog show // showFullScreen
#else
#define ShowSubDialog showFullScreen // showFullScreen
#endif

//---------------------------------------------------------
KTQAlarmClockDlg::KTQAlarmClockDlg(QQuickItem *parent)
    : QQuickItem(parent) {
    // By default, QQuickItem does not draw anything. If you subclass
    // QQuickItem to create a visual item, you will need to uncomment the
    // following line and re-implement updatePaintNode()

    // setFlag(ItemHasContents, true);
}
//------------------------------------------------
KTQAlarmClockDlg::~KTQAlarmClockDlg() {
}
//------------------------------------------------
QString KTQAlarmClockDlg::getName() const {
    return m_name;
}
//------------------------------------------------
void KTQAlarmClockDlg::setName(const QString &name) {
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}
