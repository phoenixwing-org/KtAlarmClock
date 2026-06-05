/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockDlg.cpp
 */

#include "KtAlarmClockDlg.h"

//---------------------------------------------------------
KtAlarmClockDlg::KtAlarmClockDlg(QWidget* parent)
    : QWidget(parent) {

    // setFlag(ItemHasContents, true);
}
//------------------------------------------------
KtAlarmClockDlg::~KtAlarmClockDlg() {
}
//------------------------------------------------
QString KtAlarmClockDlg::getName() const {
    return m_name;
}
//------------------------------------------------
void KtAlarmClockDlg::setName(const QString& name) {
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}
