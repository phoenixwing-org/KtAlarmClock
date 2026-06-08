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
    Q_OBJECT;
    Q_ENUMS(WorkStep);
    Q_ENUMS(WindowsVisible);
    Q_ENUMS(DialogID);
    Q_ENUMS(ActionID);

public:
    explicit KtAlarmClock(QObject* parent = nullptr);

public:
    enum WorkStep {
        None      = 0,
        WorkTime  = 1,
        WorkBreak = 2
    };

    enum WindowsVisible {
        WinHide = 0,
        WinShow = 1
    };

    enum DialogID {
        DlgRoot  = 0,
        DlgMain  = 1,
        DlgBreak = 2
    };

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
signals:
};

#endif // KtAlarmClock_H
