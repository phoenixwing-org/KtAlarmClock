/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockCmd.h
 * @version		V1.0
 * @brief		Command class
 */

#ifndef KTQAlarmClockCmd_H
#define KTQAlarmClockCmd_H

// Qt
#include <QObject>

// KTQ
#include "KTQAlarmClockUI.h"

// class pre-declare
class KTQAlarmClockDlg;
class KTQAlarmClockParam;

/**
 * @brief Class KTQAlarmClockCmd
 */
class ExportedByKTQAlarmClockUI KTQAlarmClockCmd : public QObject {
    Q_OBJECT;
    Q_DISABLE_COPY(KTQAlarmClockCmd);

public:
    explicit KTQAlarmClockCmd(QObject *parent = nullptr);
    ~KTQAlarmClockCmd() override;

private:
    KTQAlarmClockParam *m_pClockParam;
    KTQAlarmClockDlg   *m_pClockDlg;
};

#endif // KTQAlarmClockCmd_H
