/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockOverDlg.cpp
*/

#include "KTQAlarmClockOverDlg.h"
#include "ui_KTQAlarmClockOverDlg.h"

//------------------------------------------------
KTQAlarmClockOverDlg::KTQAlarmClockOverDlg(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::KTQAlarmClockOverDlg)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setWindowOpacity(0.9);

    connect(ui->pushButtonStartTalking, SIGNAL(clicked()), this, SLOT(OnStartTalking()));
    connect(ui->pushButtonFinishSpeech, SIGNAL(clicked()), this, SLOT(OnFinishSpeech()));
}
//------------------------------------------------
KTQAlarmClockOverDlg::~KTQAlarmClockOverDlg()
{
    delete ui;
}
//------------------------------------------------
QLabel *KTQAlarmClockOverDlg::GetTimeLabel()
{
    return ui->labelTime;
}
//------------------------------------------------
void KTQAlarmClockOverDlg::OnFinishSpeech()
{
    this->hide();
    emit sigFinishSpeech();
}
//------------------------------------------------
void KTQAlarmClockOverDlg::OnStartTalking()
{
    this->hide();
    emit sigStartTalking();
}
//------------------------------------------------
void KTQAlarmClockOverDlg::on_pushButtonPause_clicked()
{
    this->hide();
    emit sigPause();
}
//------------------------------------------------
void KTQAlarmClockOverDlg::on_pushButtonContinue_clicked()
{
    this->hide();
    emit sigContinue();
}
//------------------------------------------------
void KTQAlarmClockOverDlg::SetSpeechStep(SpeechStep iStep)
{
    //switches the type of step
    switch (iStep)
    {
    case SpeechFirst: //First
    {
        ui->labelStepInfor->setText("On Speech");
        ui->pushButtonContinue->show();
        //ui->pushButtonFinishSpeech->show();
        ui->pushButtonPause->show();
        ui->pushButtonStartTalking->show();
        break;
    }
    case SpeechDelay: //Delay
    {
        ui->labelStepInfor->setText("On Delay");
        ui->pushButtonContinue->show();
        //ui->pushButtonFinishSpeech->show();
        ui->pushButtonPause->show();
        ui->pushButtonStartTalking->show();
        break;
    }
    case SpeechTalking: //Delay
    {
        ui->pushButtonContinue->show();
        // ui->pushButtonFinishSpeech->show();
        ui->pushButtonPause->show();
        ui->pushButtonStartTalking->hide();
        ui->labelStepInfor->setText("On Talking");
        break;
    }        //default
    default: //Finished
    {
        ui->pushButtonContinue->hide();
        //ui->pushButtonFinishSpeech->show();
        ui->pushButtonPause->hide();
        ui->pushButtonStartTalking->hide();
        ui->labelStepInfor->setText("Speech Finished ");
        break;
    } //default
    } //switch
}
//------------------------------------------------
void KTQAlarmClockOverDlg::SetTime(const QString &iTime)
{
    ui->labelTime->setText(iTime);
}
