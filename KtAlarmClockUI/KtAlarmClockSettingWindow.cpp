/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockSettingWindow.cpp
 */

#include <QCloseEvent>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

#include "KtAlarmClock.h"
#include "KtAlarmClockParam.h"
#include "KtAlarmClockSettingWindow.h"
#include "ui_KtAlarmClockSettingWindow.h"

//------------------------------------------------------
KtAlarmClockSettingWindow::KtAlarmClockSettingWindow(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::KtAlarmClockSettingWindow) // 1
    , parameter_(nullptr)                   // 2
    , debugLocate_(true)                    // 3
    , syncingUi_(false) {                   // 4
    ui->setupUi(this);
    setModal(false); // 非模态，可与主浮窗同时操作
    connect_ui_signals();
}
//------------------------------------------------------
KtAlarmClockSettingWindow::~KtAlarmClockSettingWindow() {
    delete ui;
    ui         = nullptr; // 1
    parameter_ = nullptr; // 2
    // debugLocate_       // 3
    // syncingUi_         // 4
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::apply_seconds_value(QSlider* slider, QSpinBox* spinBox,
                                                    QLabel* timeLabel, int seconds) {
    if (!slider || !spinBox || syncingUi_) return; // 无效控件或正在批量同步

    syncingUi_ = true;

    const int clamped = qBound(slider->minimum(), seconds, slider->maximum()); // 与滑条范围一致

    if (slider->value() != clamped) slider->setValue(clamped);
    if (spinBox->value() != clamped) spinBox->setValue(clamped);
    if (timeLabel) timeLabel->setText(format_slider_value(clamped)); // 仅更新右侧 m:ss

    syncingUi_ = false;
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::bind_spin_slider(QSlider* slider, QSpinBox* spinBox,
                                                 QLabel* timeLabel) {
    if (!slider || !spinBox) return;

    spinBox->setMinimum(slider->minimum()); // 输入框范围与滑条对齐
    spinBox->setMaximum(slider->maximum());
    spinBox->setSingleStep(slider->singleStep());

    connect(slider, &QSlider::valueChanged, this, [ this, slider, spinBox, timeLabel ](int value) {
        apply_seconds_value(slider, spinBox, timeLabel, value);
    });
    connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            [ this, slider, spinBox, timeLabel ](int value) {
                apply_seconds_value(slider, spinBox, timeLabel, value);
            });
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::closeEvent(QCloseEvent* event) {
    if (debugLocate_) qDebug() << "[Setting] close";
    parameter_ = nullptr; // 解绑共享指针，避免悬空引用
    QDialog::closeEvent(event);
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::connect_ui_signals() {
    bind_spin_slider(ui->sliderWorkTime, ui->spinBoxWorkTime, ui->LabelWorkTime);
    bind_spin_slider(ui->sliderWorkBreak, ui->spinBoxWorkBreak, ui->LabelBreakTime);
    bind_spin_slider(ui->sliderTimeForce, ui->spinBoxTimeForce, ui->LabelForceTime);

    // 恢复默认
    connect(ui->pushButtonDefault, &QPushButton::clicked, this,
            &KtAlarmClockSettingWindow::restore_defaults);

    // 保存配置
    connect(ui->pushButtonSave, &QPushButton::clicked, [ this ]() {
        if (!parameter_) return;
        update_infos();
        parameter_->registerWrite();
        parameter_->dump();
    });

    connect(ui->BtnRest, &QPushButton::clicked, this,
            [ this ]() { emit action_triggered(KtAlarmClock::ActionBreak); });
    connect(ui->BtnBackward, &QPushButton::clicked, this,
            [ this ]() { emit action_triggered(KtAlarmClock::ActionBackward); });
    connect(ui->BtnPlayPause, &QPushButton::clicked, this,
            [ this ]() { emit action_triggered(KtAlarmClock::ActionPlayPause); });
    connect(ui->BtnForward, &QPushButton::clicked, this,
            [ this ]() { emit action_triggered(KtAlarmClock::ActionForward); });
    connect(ui->BtnNext, &QPushButton::clicked, this,
            [ this ]() { emit action_triggered(KtAlarmClock::ActionNextLoop); });
}
//------------------------------------------------------
QString KtAlarmClockSettingWindow::format_slider_value(int seconds) {
    if (seconds <= 0) return QStringLiteral("0:00");
    const int minutePart = seconds / 60; // 分
    const int secondPart = seconds % 60; // 秒
    return QStringLiteral("%1:%2").arg(minutePart).arg(secondPart, 2, 10, QChar('0'));
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::restore_defaults() {

    if (!parameter_) return;
    parameter_->setDefault();
    this->update_dialog();

    parameter_->registerWrite();
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::set_param(KtAlarmClockParamShared param) {
    parameter_ = param;
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::show_near(const QWidget* anchor) {
    if (!anchor) {
        show();
        return;
    }
    const QRect anchorRect = anchor->frameGeometry();
    int         posY       = anchorRect.y() + anchorRect.height();
    if (posY < 30) posY = 30; // 避免贴顶被裁切
    move(anchorRect.x(), posY);
    show();
    raise();
    activateWindow();
    if (debugLocate_) qDebug() << "[Setting] show_near anchor=" << anchorRect << "dlgPos=" << pos();
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::update_dialog() {
    if (!parameter_) return;

    apply_seconds_value(ui->sliderWorkTime, ui->spinBoxWorkTime, ui->LabelWorkTime,
                        parameter_->WorkTime);
    apply_seconds_value(ui->sliderWorkBreak, ui->spinBoxWorkBreak, ui->LabelBreakTime,
                        parameter_->WorkBreak);
    apply_seconds_value(ui->sliderTimeForce, ui->spinBoxTimeForce, ui->LabelForceTime,
                        parameter_->TimeForce);

    if (debugLocate_) qDebug() << "[Setting] update_dialog";
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::update_infos() {
    if (!parameter_) return;

    parameter_->WorkTime  = ui->spinBoxWorkTime->value(); // 以 spinBox 为准
    parameter_->WorkBreak = ui->spinBoxWorkBreak->value();
    parameter_->TimeForce = ui->spinBoxTimeForce->value();

    if (debugLocate_)
        qDebug() << "[Setting] update_infos Work=" << parameter_->WorkTime
                 << "Break=" << parameter_->WorkBreak << "Force=" << parameter_->TimeForce;
}
