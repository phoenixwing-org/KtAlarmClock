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
class KTQAlarmClockCore;
class KTQAlarmClockDlg;
class KTQAlarmClockParam;
class QGuiApplication;
class QQmlApplicationEngine;

/**
 * @brief Class KTQAlarmClockCmd
 */
class ExportedByKTQAlarmClockUI KTQAlarmClockCmd : public QObject {
    Q_OBJECT;
    Q_DISABLE_COPY(KTQAlarmClockCmd);

public:
    explicit KTQAlarmClockCmd(QObject *parent = nullptr);
    ~KTQAlarmClockCmd() override;

public:
    /**
     * @brief Print Message
     * @param[in] iMsg messge
     */
    void debug(const QString &iMsg);

    /**
     * @brief Build Dialog
     */
    int BuildDialog(QQmlApplicationEngine *engine);

private:
    KTQAlarmClockCore  *m_pClockCore;
    KTQAlarmClockParam *m_pClockParam;
    KTQAlarmClockDlg   *m_pClockDlg;
};

#endif // KTQAlarmClockCmd_H
