/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     Apache-2.0
 * @author      Kuntai
 * @file        KtAlarmClockCmd.h
 * @version		V1.0
 * @brief		Command class
 */

#ifndef KtAlarmClockCmd_H
#define KtAlarmClockCmd_H

// Qt
#include <QObject>

// Kt
#include "KtAlarmClockParam.h"
#include "KtAlarmClockUI.h"

// class pre-declare
class KtAlarmClockController;
class KtLockScreenManager;
class KtSingleInstanceGuard;
class QGuiApplication;

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
     * @brief Build Dialog
     */
    int build();

    /**
     * @brief Print Message
     * @param[in] iMsg message
     */
    void debug(const QString& iMsg);

    void setExePath(const QString& iPath) {
        m_ExePath = iPath;
    };

public slots:
    /**
     * @brief Force terminate process when normal event-loop shutdown is unavailable
     */
    void forceQuit();

    /**
     * @brief auto start
     */
    int setAutoStart(bool iValue);

private:
    KtAlarmClockParamShared parameter;
    KtAlarmClockController*   m_pController;
    KtSingleInstanceGuard*    instanceGuard_;
    QString                   m_ExePath;
};

#endif // KtAlarmClockCmd_H
