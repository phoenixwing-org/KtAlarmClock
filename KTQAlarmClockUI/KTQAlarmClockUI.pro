TEMPLATE = lib
TARGET = KTQAlarmClockUI
QT += core qml quick
QT       += multimedia

DEFINES += KTQAlarmClockUI_EXPORTS

CONFIG += plugin c++11

# DESTDIR: define the out exe or dll folder
CONFIG(debug, debug|release){
    DESTDIR=$$PWD/../../bin/debug
    MOC_DIR = "$$OUT_PWD/tmp/debug/.moc"
    OBJECTS_DIR =  "$$OUT_PWD/tmp/debug/.obj"
    UI_DIR =  "$$OUT_PWD/tmp/debug/.ui"
    RCC_DIR =  "$$OUT_PWD/tmp/debug/.qrc"
}
CONFIG(release, debug|release){
    DESTDIR=$$PWD/../../bin/release
    MOC_DIR = "$$OUT_PWD/tmp/release/.moc"
    OBJECTS_DIR =  "$$OUT_PWD/tmp/release/.obj"
    UI_DIR =  "$$OUT_PWD/tmp/release/.ui"
    RCC_DIR =  "$$OUT_PWD/tmp/release/.qrc"
}
# message(KTQAlarmClockUI.OUT_PWD=$$OUT_PWD)

TARGET = $$qtLibraryTarget($$TARGET)
uri = cn.kuntaisoft.KTQAlarmClockDlg

# Input
SOURCES += \
        KTQAlarmClockCore.cpp \
        KTQAlarmClockDlg.cpp \
        KTQAlarmClockParam.cpp \
        KTQAlarmClockPlugin.cpp \
        KTQAlarmClockCmd.cpp

HEADERS += \
        KTQAlarmClockCore.h \
        KTQAlarmClockDlg.h \
        KTQAlarmClockParam.h \
        KTQAlarmClockPlugin.h \
        KTQAlarmClockCmd.h

DISTFILES = qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) "$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)" "$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

RESOURCES += \
    main.qrc
