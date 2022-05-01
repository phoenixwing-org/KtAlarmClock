/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockDlg.cpp
 */

#include <QDateTime>
#include <QMessageBox>

#include "KTQAlarmClockDlg.h"

#define ForTestTimter 0
#if ForTestTimter
#define ShowSubDialog show // showFullScreen
#else
#define ShowSubDialog showFullScreen // showFullScreen
#endif

//---------------------------------------------------------
KTQAlarmClockDlg::KTQAlarmClockDlg(QObject *parent)
    : QObject(parent)
    , m_DelayCount(0)
    , m_Step(KTQ::SpeechFinished)
    , m_Punish(0)
    , m_PunishTarget(15)
    , m_pSoundEffect(NULL) {

    // m_pTimeLabel = ui->labelTime; // set label
    m_pTimer = new QTimer();
    m_pause  = false;

    // for test only
#if ForTestTimter
    // ui->doubleSpinBoxTimeCount->setValue(0.1);  // for test
    // ui->doubleSpinBoxDelayCount->setValue(0.0); // for test
    // ui->doubleSpinBoxWholeCount->setValue(0.3); // for test
#endif
    OnInitNumber(); // show time

    // connect(m_pTimer, SIGNAL(timeout()), this, SLOT(SubTime()));

    // connect(ui->doubleSpinBoxWholeCount, SIGNAL(valueChanged(double)), this,
    // SLOT(OnInitNumber())); connect(ui->pushButtonStart, SIGNAL(clicked()), this,
    // SLOT(OnStart())); connect(ui->pushButtonQuestion, SIGNAL(clicked()), this,
    // SLOT(OnNextStep())); connect(ui->pushButtonPause, SIGNAL(clicked()), this,
    // SLOT(OnNextStep())); connect(ui->pushButtonReset, SIGNAL(clicked()), this,
    // SLOT(OnFinishSpeech())); connect(ui->pushButtonClose, SIGNAL(clicked()), this,
    // SLOT(OnClose()));
    //
    // connect(m_pTimeOverDlg, SIGNAL(sigStartTalking()), this, SLOT(OnStartTalking()));
    // connect(m_pTimeOverDlg, SIGNAL(sigFinishSpeech()), this, SLOT(OnFinishSpeech()));
    // connect(m_pTimeOverDlg, SIGNAL(sigPause()), this, SLOT(OnPause()));
    // connect(m_pTimeOverDlg, SIGNAL(sigContinue()), this, SLOT(OnContinue()));

    // Bell
    // QString wavPath = qApp->applicationDirPath();
    // wavPath.append("/wav/didi.wav");

    // m_pSoundEffect = new QSoundEffect();
    // m_pSoundEffect->setLoopCount(5);
    // m_pSoundEffect->setSource(QUrl::fromLocalFile(wavPath));
}
//------------------------------------------------
KTQAlarmClockDlg::~KTQAlarmClockDlg() {
    delete m_pTimer;
    delete m_pSoundEffect;
}
//------------------------------------------------
void KTQAlarmClockDlg::OnInitNumber() {
    SetLCDNumColor("color: rgb(0,0,0)");
    // SetTimeCount(ui->doubleSpinBoxTimeCount->value());
    // SetTimeDelay(ui->doubleSpinBoxDelayCount->value());
    // SetTimeWhole(ui->doubleSpinBoxWholeCount->value());

    // m_PunishTarget    = ui->doubleSpinBoxPunish->value(); // set punish
    m_DelayCountTotal = m_DelayCount; // copy
    m_Punish          = 0.0;          // set punish

    ShowTime();
}
//------------------------------------------------
void KTQAlarmClockDlg::OnContinue() {
    this->ShowParamDialog(false); // show param dialog
}
//------------------------------------------------
void KTQAlarmClockDlg::OnClose() {
    // this->hide();
    // this->close();
}
//------------------------------------------------
void KTQAlarmClockDlg::OnFinishSpeech() {
    this->ShowParamDialog(false); // hide ParamDialog
    m_Step = KTQ::SpeechFinished;
    m_pTimer->stop();
    OnInitNumber();
    // ui->pushButtonStart->setEnabled(true);
    // ui->pushButtonPause->setEnabled(false);
    // ui->doubleSpinBoxTimeCount->setEnabled(true);  // enable
    // ui->doubleSpinBoxDelayCount->setEnabled(true); // enable
    // ui->doubleSpinBoxWholeCount->setEnabled(true); // enable
    // ui->doubleSpinBoxPunish->setEnabled(true);     // enable

    // ui->widget->setVisible(true);

    this->ShowTopmost(); // top most
}
//------------------------------------------------
void KTQAlarmClockDlg::OnNextStep() {
    this->ShowParamDialog(true); // show param dialog
}
//------------------------------------------------
void KTQAlarmClockDlg::OnPause() {
    this->ShowParamDialog(false); // hide ParamDialog
    m_pause = true;
    m_pTimer->stop(); // stop
    // ui->pushButtonStart->setEnabled(true);
    // ui->pushButtonPause->setEnabled(false);
    // ui->widget->setVisible(true);

    this->ShowTopmost(); // top most
    // this->move(m_PosX, 0);
}
//------------------------------------------------
void KTQAlarmClockDlg::OnQuestion() {
    // not use
}
//------------------------------------------------
void KTQAlarmClockDlg::OnStart() {
    this->ShowParamDialog(false); // hide ParamDialog
    // m_PosX = this->pos().x();     // x pos

    if (m_WholeCount < m_TimeCount + m_DelayCount) {
        m_WholeCount = m_TimeCount + m_DelayCount;
    }

    // ui->widget->setVisible(false);
    if (m_pause) {
        m_pause = false;
    } else {
        // ui->labelPunish->hide(); // hide
        OnInitNumber(); // show time
    }

    // for buttons active
    if (!m_pTimer->isActive()) {
        m_pTimer->start(1000);
    }

    // ui->pushButtonStart->setEnabled(false);
    // ui->pushButtonPause->setEnabled(true);

    // ui->doubleSpinBoxTimeCount->setEnabled(false);  // disable
    // ui->doubleSpinBoxDelayCount->setEnabled(false); // disable
    // ui->doubleSpinBoxWholeCount->setEnabled(false); // disable
    // ui->doubleSpinBoxPunish->setEnabled(false);     // disable

    this->ShowTopmost(); // top most

    // this->move(m_PosX, 0);
}
//------------------------------------------------
void KTQAlarmClockDlg::OnStartTalking() {
    this->ShowParamDialog(false); // hide ParamDialog
    m_Step = KTQ::SpeechTalking;
    ShowTime();
    // ui->pushButtonStart->setEnabled(false);
    // ui->pushButtonPause->setEnabled(true);
    // ui->widget->setVisible(false);

    this->ShowTopmost(); // top most
    // this->move(m_PosX, 0);
}
//------------------------------------------------
void KTQAlarmClockDlg::reject() {
    // this->show();
}
//------------------------------------------------
void KTQAlarmClockDlg::SetLCDNumColor(const QString &iColor) {
    // m_pTimeLabel->setStyleSheet(iColor);
}
//------------------------------------------------
void KTQAlarmClockDlg::SetTimeCount(double time) {
    m_TimeCount = time * 60;
    if (m_TimeCount > 0) m_Step = KTQ::SpeechFirst; // first
}
//------------------------------------------------
void KTQAlarmClockDlg::SetTimeDelay(double time) {
    m_DelayCount = time * 60;
}
//------------------------------------------------
void KTQAlarmClockDlg::SetTimeWhole(double time) {
    m_WholeCount = time * 60;
}
//------------------------------------------------
void KTQAlarmClockDlg::ShowParamDialog(bool iIsShow) {
    if (iIsShow) {
        // m_pTimeOverDlg->SetSpeechStep(m_Step); // set step
        // m_pTimeOverDlg->ShowSubDialog();       // show information dialog
        // m_pTimeLabel = m_pTimeOverDlg->GetTimeLabel();
    } else {
        // m_pTimeOverDlg->hide();
        // m_pTimeLabel = ui->labelTime;
    }
    ShowTime();
    this->UpdateSensitivity();
}
//------------------------------------------------
void KTQAlarmClockDlg::ShowTime() {
    // m_pTimeOverDlg->SetSpeechStep(m_Step); // set step

    int time = 0; // time for show

    // switches the type of step
    switch (m_Step) {
        case KTQ::SpeechFirst: { // First
            time = m_TimeCount;
            if (time == 59 || time == 57 || time == 55 || time == 53 || time == 51 || time == 3 ||
                time == 1) {
                SetLCDNumColor("background-color: rgb(255,0,0)");
            } else if (time == 58 || time == 56 || time == 54 || time == 52 || time == 50 ||
                       time == 2 || time == 0) {
                SetLCDNumColor("background-color: rgb(255,255,255)");
            }
            break;
        }
        case KTQ::SpeechDelay: { // Delay
            time = m_DelayCount;
            if (time % 2 == 0) {
                SetLCDNumColor("background-color: rgb(255,255,0)");
            } else {
                SetLCDNumColor("background-color: rgb(255,255,200)");
            }

            break;
        }
        default: { // talking
            SetLCDNumColor("background-color: rgb(255,255,255)");
            time = m_WholeCount;
            break;
        } // default
    }     // switch

    if (m_Punish >= 0.1) {
        QString punishStr = QString::number(m_Punish, 'f', 1);
        // ui->labelPunish->setText(punishStr);
    }
    if (time < 0) return; // error

    int m = time / 60;
    int s = time % 60;

    QString timeStr = "%1:%2";
    if (m / 10 == 0 && s / 10 == 0)
        timeStr = "0%1:0%2";
    else if (m / 10 == 0 && s / 10 != 0)
        timeStr = "0%1:%2";
    else if (m / 10 != 0 && s / 10 == 0)
        timeStr = "%1:0%2";

    timeStr = timeStr.arg(m).arg(s);
    // m_pTimeLabel->setText(timeStr);
    // m_pTimeOverDlg->SetTime(timeStr);
}
//------------------------------------------------
void KTQAlarmClockDlg::ShowTopmost() {
    // this->hide();
    // setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    // this->show();
}
//------------------------------------------------
void KTQAlarmClockDlg::SubTime() {
    // switches the type of step
    switch (m_Step) {
        case KTQ::SpeechFirst: { // First
            if (m_TimeCount < 1) {
                if (m_DelayCountTotal >= 0) {
                    m_Step = KTQ::SpeechDelay; // next step
                } else {
                    m_Step = KTQ::SpeechTalking; // next step
                    break;
                }
            } else {
                m_TimeCount--; // time--
                // if (m_TimeCount % 10 == 0) this->show(); // show
                break;
            }
        }
        case KTQ::SpeechDelay: { // Delay

            m_DelayCount--;             // time--
            if (m_DelayCountTotal != 0) // if =0,
            {
                m_Punish = m_PunishTarget * (m_DelayCountTotal - m_DelayCount) / m_DelayCountTotal;
            }
            if (m_DelayCount <= 0) {
                m_Step = KTQ::SpeechTalking; // next step

                m_pSoundEffect->play();
                this->ShowParamDialog(true); // show param dialog
            }
            break;
        }
        case KTQ::SpeechTalking: { // talking
            if (m_WholeCount > 0) {
                // if (m_TimeCount % 10 == 0) this->show(); // show
                break;
            }
        }
        default: {
            m_Step = KTQ::SpeechFinished; // next step
            m_pTimer->stop();
            m_pSoundEffect->play();
            this->ShowParamDialog(true); // show param dialog

            break;
        }
    } // switch

    // ui->labelPunish->setVisible(m_Punish > 0);
    m_WholeCount--; // whole --
    ShowTime();
}
//---------------------------------------------------
void KTQAlarmClockDlg::UpdateSensitivity() {
}
