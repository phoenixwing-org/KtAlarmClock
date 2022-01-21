#ifndef KTQAlarmClockDlg_H
#define KTQAlarmClockDlg_H
/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockDlg.h
 * @brief       main dialog
*/

#include <QDialog>
#include <QTimer>
#include <QPoint>
#include <QLabel>
#include <QMouseEvent>
#include <QSoundEffect> // for Qt 6

#include "KTQAlarmClockOverDlg.h"

namespace Ui
{
    class KTQAlarmClockDlg;
}

class KTQAlarmClockDlg : public QDialog
{
    Q_OBJECT

public:
    explicit KTQAlarmClockDlg(QWidget *parent = 0);
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

private:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

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
    Ui::KTQAlarmClockDlg *ui;
    int m_TimeCount;
    int m_DelayCount;
    int m_DelayCountTotal;
    int m_WholeCount; // for whole
    QTimer *m_pTimer;
    bool m_pause;
    KTQAlarmClockOverDlg *m_pTimeOverDlg;
    int m_PosX;
    SpeechStep m_Step;
    double m_Punish;       // punish
    double m_PunishTarget; // punish target.
    QPoint dPos;           //Position
    QLabel *m_pTimeLabel;
    QSoundEffect *m_pSoundEffect; //QT6.0 QSound is replaced by QSoundEffect
};

#endif // KTQAlarmClockDlg_H
