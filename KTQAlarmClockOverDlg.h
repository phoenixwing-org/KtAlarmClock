#ifndef KTQAlarmClockOverDlg_H
#define KTQAlarmClockOverDlg_H
/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockOverDlg.h
 * @brief       dialog after time is over.
*/

#include <QWidget>
#include <QLabel>

namespace Ui
{
    class KTQAlarmClockOverDlg;
}
enum SpeechStep
{
    SpeechFirst = 0,
    SpeechDelay = 1,
    SpeechTalking = 2,
    SpeechFinished = 3
};

class KTQAlarmClockOverDlg : public QWidget
{
    Q_OBJECT

public:
    explicit KTQAlarmClockOverDlg(QWidget *parent = 0);
    ~KTQAlarmClockOverDlg();

public:
    QLabel *GetTimeLabel();

    void SetSpeechStep(SpeechStep iStep);

    void SetTime(const QString &iTime);

signals:
    void sigFinishSpeech();

    void sigStartTalking();

    void sigPause();

    void sigContinue();

private slots:

    void OnFinishSpeech();

    void OnStartTalking();

    void on_pushButtonContinue_clicked();

    void on_pushButtonPause_clicked();

private:
    Ui::KTQAlarmClockOverDlg *ui;
};

#endif // KTQAlarmClockOverDlg_H
