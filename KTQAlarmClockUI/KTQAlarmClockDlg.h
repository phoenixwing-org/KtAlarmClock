#ifndef KTQAlarmClockDlg_H
#define KTQAlarmClockDlg_H
/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockDlg.h
 * @brief       main dialog
 */

#include <QPoint>
#include <QSoundEffect>
#include <QTimer>

// KTQ
#include "KTQAlarmClockParam.h"

class KTQAlarmClockDlg : public QObject {
    Q_OBJECT

public:
    explicit KTQAlarmClockDlg(QObject *parent = 0);
    ~KTQAlarmClockDlg();

public:
    void reject();

    void SetLCDNumColor(const QString &iColor);

    void SetTimeCount(double time);

    void SetTimeDelay(double time);

    void SetTimeWhole(double time);

    void ShowParamDialog(bool iIsShow);

    void ShowTopmost();

    void UpdateSensitivity();

private slots:

    void OnContinue();

    void OnInitNumber();

    void OnNextStep();

    void OnClose();

    void OnFinishSpeech();

    void OnStartTalking();

    void OnPause();

    void ShowTime();

    void OnStart();

    void SubTime();

    void OnQuestion();

private:
    int             m_TimeCount;
    int             m_DelayCount;
    int             m_DelayCountTotal;
    int             m_WholeCount; // for whole
    QTimer         *m_pTimer;
    bool            m_pause;
    int             m_PosX;
    KTQ::SpeechStep m_Step;
    double          m_Punish;       // punish
    double          m_PunishTarget; // punish target.
    QPoint          dPos;           // Position
    QSoundEffect   *m_pSoundEffect; // QT6.0 QSound is replaced by QSoundEffect
};

#endif // KTQAlarmClockDlg_H
