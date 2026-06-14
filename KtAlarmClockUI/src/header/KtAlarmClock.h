/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClock.h
 */
#ifndef KtAlarmClock_H
#define KtAlarmClock_H

#include <QObject>

class KtAlarmClock : public QObject {
    Q_OBJECT

public:
    explicit KtAlarmClock(QObject* parent = nullptr);

public:
    enum WorkStep {
        None      = 0,
        WorkTime  = 1,
        WorkBreak = 2
    };
    Q_ENUM(WorkStep)

    enum WindowsVisible {
        WinHide = 0,
        WinShow = 1
    };
    Q_ENUM(WindowsVisible)

    enum DialogID {
        DlgRoot  = 0,
        DlgMain  = 1,
        DlgBreak = 2
    };
    Q_ENUM(DialogID)

    enum ActionID {
        ActionNone      = 0,
        ActionClose     = 1,
        ActionMainDlg   = 2,
        ActionBreak     = 3,
        ActionHelp      = 4,
        ActionNextLoop  = 5,
        ActionPlayPause = 6,
        ActionForward   = 7,
        ActionBackward  = 8,
        ActionKtWeb     = 9,
    };
    Q_ENUM(ActionID)
signals:
};

#endif // KtAlarmClock_H
