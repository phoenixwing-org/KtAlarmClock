#-------------------------------------------------
# @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
# @license     LGPL 3.0
# @author      Kuntai
# @file        KTQAlarmClock.pro
#-------------------------------------------------

QT       += core gui widgets
QT       += multimedia

TARGET = KTQAlarmClock
TEMPLATE = app

SOURCES += main.cpp\
    KTQAlarmClockDlg.cpp \
    KTQAlarmClockOverDlg.cpp

HEADERS  += KTQAlarmClockDlg.h \
    KTQAlarmClockOverDlg.h

FORMS    += KTQAlarmClockDlg.ui \
    KTQAlarmClockOverDlg.ui

RESOURCES += \
    KTQAlarmClock.qrc
	
RC_FILE = KTQAlarmClock.rc
