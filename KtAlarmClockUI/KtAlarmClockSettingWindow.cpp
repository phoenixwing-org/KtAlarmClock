/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockSettingWindow.cpp
 */

// Qt
#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QFontMetrics>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QScreen>
#include <QSizePolicy>
#include <QSlider>
#include <QStyle>
#include <QWindow>

#include "KtAlarmClock.h"
#include "KtAlarmClockParam.h"
#include "KtAlarmClockSettingWindow.h"
#include "KtDurationEdit.h"
#include "KtScreenUtil.h"
#include "ui_KtAlarmClockSettingWindow.h"

//------------------------------------------------------
KtAlarmClockSettingWindow::KtAlarmClockSettingWindow(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::KtAlarmClockSettingWindow) // 1
    , parameter_(nullptr)                   // 2
    , debugLocate_(false)                   // 3
    , syncingUi_(false) {                   // 4
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("护眼闹钟 - 设置 (%1)").arg(QLatin1String(VERSION_KtAlarmClock)));

    for (QPushButton* btn : findChildren<QPushButton*>()) {
        btn->setAutoDefault(false);
        btn->setDefault(false);
    }

    apply_layout_metrics();
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
void KtAlarmClockSettingWindow::apply_layout_metrics() {
    QFont uiFont = font();
    uiFont.setPointSize(10);
    setFont(uiFont);

    const QFontMetrics fm(uiFont);
    const int          rowMinH   = fm.height() + 14;
    int                labelColW = 0;
    for (QLabel* label : {ui->LabelWork, ui->LabelBreak, ui->LabelForce})
        labelColW = qMax(labelColW, fm.horizontalAdvance(label->text()));
    const int durationColW = fm.horizontalAdvance(QStringLiteral("90:00")) + 12;

    ui->verticalLayout->setContentsMargins(16, 16, 16, 16);
    ui->verticalLayout->setSpacing(14);
    ui->footerLayout->setSpacing(10);
    ui->horizontalLayout->setSpacing(12);

    auto* grid = ui->gridLayout;
    grid->setHorizontalSpacing(14);
    grid->setVerticalSpacing(18);
    grid->setColumnMinimumWidth(1, durationColW);
    grid->setColumnStretch(0, 0);
    grid->setColumnStretch(1, 0);
    grid->setColumnStretch(2, 1);

    for (QLabel* label : {ui->LabelWork, ui->LabelBreak, ui->LabelForce}) {
        label->setFont(uiFont);
        label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    }
    for (KtDurationEdit* edit :
         {ui->durationWorkTime, ui->durationWorkBreak, ui->durationTimeForce}) {
        edit->setFont(uiFont);
        edit->setMinimumHeight(rowMinH);
        edit->setFixedWidth(durationColW);
        edit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }
    for (QSlider* slider : {ui->sliderWorkTime, ui->sliderWorkBreak, ui->sliderTimeForce}) {
        slider->setMinimumHeight(rowMinH);
    }

    const int iconBtn =
        qMax(36, QApplication::style()->pixelMetric(QStyle::PM_ButtonIconSize) + 12);
    const int iconPx = qMax(20, iconBtn - 12);
    for (QPushButton* btn : {ui->pushButtonRest, ui->pushButtonBackward, ui->pushButtonPlayPause,
                             ui->pushButtonForward, ui->pushButtonNext}) {
        btn->setFixedSize(iconBtn, iconBtn);
        btn->setIconSize(QSize(iconPx, iconPx));
        btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }
    ui->pushButtonRest->setToolTip(QStringLiteral("立刻休息"));

    const int actionRowH = fm.height() + 22;
    ui->pushButtonDefault->setMinimumHeight(actionRowH);
    ui->pushButtonSave->setMinimumHeight(actionRowH);
    ui->pushButtonDefault->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->pushButtonSave->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->verticalLayout->setStretch(0, 1);
    ui->verticalLayout->setStretch(1, 0);
    ui->verticalLayout->setStretch(2, 0);

    const int minDlgW = labelColW + durationColW + grid->horizontalSpacing() * 2 + 240;
    const int minDlgH = 3 * rowMinH + 2 * grid->verticalSpacing() + iconBtn + actionRowH +
                        ui->verticalLayout->contentsMargins().top() +
                        ui->verticalLayout->contentsMargins().bottom() +
                        ui->verticalLayout->spacing() * 2 + 32;
    setMinimumSize(minDlgW, minDlgH);
    resize(qMax(width(), minDlgW), qMax(height(), minDlgH));
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::apply_seconds_value(QSlider* slider, KtDurationEdit* durationEdit,
                                                    int seconds) {
    if (!slider || !durationEdit || syncingUi_) return;

    syncingUi_ = true;

    const int clamped = qBound(slider->minimum(), seconds, slider->maximum());
    if (slider->value() != clamped) slider->setValue(clamped);
    durationEdit->set_seconds(clamped);

    syncingUi_ = false;
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::bind_duration_slider(QSlider*        slider,
                                                     KtDurationEdit* durationEdit) {
    if (!slider || !durationEdit) return;

    durationEdit->set_range(slider->minimum(), slider->maximum());

    connect(slider, &QSlider::valueChanged, this, [ this, slider, durationEdit ](int value) {
        apply_seconds_value(slider, durationEdit, value);
    });
    connect(durationEdit, &KtDurationEdit::seconds_changed, this,
            [ this, slider, durationEdit ](int value) {
                apply_seconds_value(slider, durationEdit, value);
            });
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::closeEvent(QCloseEvent* event) {
    if (debugLocate_) qDebug() << "[Setting] close";
    parameter_ = nullptr;
    hide();
    event->ignore(); // 非模态：仅隐藏，保留实例供再次打开复用
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::connect_ui_signals() {
    bind_duration_slider(ui->sliderWorkTime, ui->durationWorkTime);
    bind_duration_slider(ui->sliderWorkBreak, ui->durationWorkBreak);
    bind_duration_slider(ui->sliderTimeForce, ui->durationTimeForce);

    connect(ui->pushButtonDefault, &QPushButton::clicked, this,
            &KtAlarmClockSettingWindow::restore_defaults);

    connect(ui->pushButtonSave, &QPushButton::clicked, [ this ]() {
        if (!parameter_) return;
        update_infos();
        parameter_->registerWrite();
        parameter_->dump();
    });

    connect(ui->pushButtonRest, &QPushButton::clicked, this,
            [ this ]() { emit action_triggered(KtAlarmClock::ActionBreak); });
    connect(ui->pushButtonBackward, &QPushButton::clicked, this,
            [ this ]() { emit action_triggered(KtAlarmClock::ActionBackward); });
    connect(ui->pushButtonPlayPause, &QPushButton::clicked, this,
            [ this ]() { emit action_triggered(KtAlarmClock::ActionPlayPause); });
    connect(ui->pushButtonForward, &QPushButton::clicked, this,
            [ this ]() { emit action_triggered(KtAlarmClock::ActionForward); });
    connect(ui->pushButtonNext, &QPushButton::clicked, this,
            [ this ]() { emit action_triggered(KtAlarmClock::ActionNextLoop); });
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::restore_defaults() {
    if (!parameter_) return;
    parameter_->setDefault();
    update_dialog();
    parameter_->registerWrite();
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::set_param(KtAlarmClockParamShared param) {
    parameter_ = param;
    update_dialog();
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::show_near(const QPoint& globalRef) {
    adjustSize();
    move(KtScreenUtil::place_popup(globalRef, size(), true));
    show();
    KtScreenUtil::assign_screen(this, globalRef);
    const QPoint clamped = KtScreenUtil::place_popup(globalRef, frameGeometry().size(), true);
    if (pos() != clamped) move(clamped);
    raise();
    activateWindow();
    if (debugLocate_)
        qDebug() << "[Setting] show_near ref=" << globalRef << "dlgPos=" << pos() << "screen="
                 << (windowHandle() && windowHandle()->screen() ? windowHandle()->screen()->name()
                                                                : QString());
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::update_dialog() {
    if (!parameter_) return;

    apply_seconds_value(ui->sliderWorkTime, ui->durationWorkTime, parameter_->WorkTime);
    apply_seconds_value(ui->sliderWorkBreak, ui->durationWorkBreak, parameter_->WorkBreak);
    apply_seconds_value(ui->sliderTimeForce, ui->durationTimeForce, parameter_->TimeForce);

    if (debugLocate_) qDebug() << "[Setting] update_dialog";
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::update_infos() {
    if (!parameter_) return;

    ui->durationWorkTime->commit_text();
    ui->durationWorkBreak->commit_text();
    ui->durationTimeForce->commit_text();

    parameter_->WorkTime  = ui->durationWorkTime->get_seconds();
    parameter_->WorkBreak = ui->durationWorkBreak->get_seconds();
    parameter_->TimeForce = ui->durationTimeForce->get_seconds();

    if (debugLocate_)
        qDebug() << "[Setting] update_infos Work=" << parameter_->WorkTime
                 << "Break=" << parameter_->WorkBreak << "Force=" << parameter_->TimeForce;
}
//------------------------------------------------------
void KtAlarmClockSettingWindow::update_play_pause_button(bool running) {
    if (running) {
        ui->pushButtonPlayPause->setIcon(QIcon(QStringLiteral(":/image/pause.svg")));
        ui->pushButtonPlayPause->setToolTip(QStringLiteral("暂停"));
    }
    else {
        ui->pushButtonPlayPause->setIcon(QIcon(QStringLiteral(":/image/play.svg")));
        ui->pushButtonPlayPause->setToolTip(QStringLiteral("播放"));
    }
}
