#-------------------------------------------------
# @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
# @license     LGPL 3.0
# @author      Kuntai
# @file        KTQAlarmClock.pro
#-------------------------------------------------

QT       += core gui widgets quick
QT       += multimedia

CONFIG += c++11

TARGET = KTQAlarmClock
TEMPLATE = app

SOURCES +=\
    KTQAlarmClockDlg.cpp \
    KTQAlarmClockOverDlg.cpp \
    main.cpp

HEADERS  += KTQAlarmClockDlg.h \
    KTQAlarmClockOverDlg.h

FORMS    += KTQAlarmClockDlg.ui \
    KTQAlarmClockOverDlg.ui

RESOURCES += \
    main.qrc

RC_FILE = main.rc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
