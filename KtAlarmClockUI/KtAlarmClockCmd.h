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

// Kt
#include "KtAlarmClockCore.h"
#include "KtAlarmClockUI.h"

// class pre-declare
class KtAlarmClockController;
class KtAlarmClockDlg;
class KtLockScreenManager;
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
     * @brief Print Message
     * @param[in] iMsg message
     */
    void debug(const QString& iMsg);

    /**
     * @brief Build Dialog
     */
    int build();

    /**
     * @brief Give My Panel
     */
    KtAlarmClockDlg* giveMyPanel() const;

    void setExePath(const QString& iPath) {
        m_ExePath = iPath;
    };

public slots:

    /**
     * @brief auto start
     */
    int setAutoStart(bool iValue);

    /**
     * @brief Force terminate process (bypass QML event loop cleanup)
     */
    void forceQuit();

private:
    KtAlarmClockCoreShared  core;
    KtAlarmClockParamShared parameter;
    KtAlarmClockDlg*        dialog;
    KtAlarmClockController* m_pController;
    QString                 m_ExePath;
};

#endif // KtAlarmClockCmd_H
