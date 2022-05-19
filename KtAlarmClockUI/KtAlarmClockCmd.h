/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockCmd.h
 * @version		V1.0
 * @brief		Command class
 */

#ifndef KtAlarmClockCmd_H
#define KtAlarmClockCmd_H

// Qt
#include <QObject>
#include <QQuickItem>
// kt
#include "ktCoreDefine.h"
#include "ktErrorCode.h"

// Kt
#include "KtAlarmClockUI.h"

// class pre-declare
class KtAlarmClockCore;
class KtAlarmClockDlg;
class KtAlarmClockParam;
class QGuiApplication;
class QQmlApplicationEngine;

/**
 * @brief Class KtAlarmClockCmd
 */
class ExportedByKtAlarmClockUI KtAlarmClockCmd : public QObject {
    Q_OBJECT;
    Q_DISABLE_COPY(KtAlarmClockCmd);

public:
    explicit KtAlarmClockCmd(QObject* parent = nullptr);
    ~KtAlarmClockCmd() override;

public:
    /**
     * @brief Print Message
     * @param[in] iMsg messge
     */
    void debug(const QString& iMsg);

    /**
     * @brief Build Dialog
     */
    ktErrorCode BuildDialog(QQmlApplicationEngine* engine);

    /**
     * @brief Give My Panel
     */
    QQuickItem* GiveMyPanel() const;

    void SetExePath(const QString& iPath) {
        m_ExePath = iPath;
    };

public slots:

    /**
     * @brief start the clock
     */
    int onStart();

    /**
     * @brief auto start
     */
    int SetAutoStart(bool iValue);

private:
    KtAlarmClockCore*  m_pClockCore;
    KtAlarmClockParam* m_pClockParam;
    KtAlarmClockDlg*   m_pClockDlg;
    QString            m_ExePath;
};

#endif // KtAlarmClockCmd_H
