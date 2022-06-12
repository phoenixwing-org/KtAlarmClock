TEMPLATE = lib
TARGET = KtAlarmClockUI
QT += core qml quick multimedia

DEFINES += KtAlarmClockUI_EXPORTS

CONFIG += plugin c++11

# DESTDIR: define the out exe or dll folder
CONFIG(debug, debug|release){
    DESTDIR = $$PWD/../../bin/debug
    MOC_DIR = "$$OUT_PWD/tmp/debug/.moc"
    OBJECTS_DIR =  "$$OUT_PWD/tmp/debug/.obj"
    UI_DIR  = "$$OUT_PWD/tmp/debug/.ui"
    RCC_DIR = "$$OUT_PWD/tmp/debug/.qrc"
}
CONFIG(release, debug|release){
    DESTDIR = $$PWD/../../bin/release
    MOC_DIR = "$$OUT_PWD/tmp/release/.moc"
    OBJECTS_DIR = "$$OUT_PWD/tmp/release/.obj"
    UI_DIR  = "$$OUT_PWD/tmp/release/.ui"
    RCC_DIR = "$$OUT_PWD/tmp/release/.qrc"
}

# KT_BASE_INCLUDE for deferent system
unix {
    KT_BASE_INCLUDE = $$(KT_INCLUDES)/Linux/include
}
macx {
    KT_BASE_INCLUDE = $$(KT_INCLUDES)/Mac/include
}
win32 {
    KT_BASE_INCLUDE = $$(KT_INCLUDES)\Windows\include
}

# message(KT_INCLUDES=$$(KT_INCLUDES))
# message(KT_BASE_INCLUDE=$${KT_BASE_INCLUDE})

INCLUDEPATH += \
         $${KT_BASE_INCLUDE} \

# TARGET = $$qtLibraryTarget($$TARGET)
uri = cn.kuntaisoft.KtAlarmClockDlg

# Input
SOURCES += \
        KtAlarmClockCore.cpp \
        KtAlarmClockDlg.cpp \
        KtAlarmClockParam.cpp \
        KtAlarmClockPlugin.cpp \
        KtAlarmClockCmd.cpp \
        KtAlarmClock.cpp

HEADERS += \
        KtAlarmClockCore.h \
        KtAlarmClockDlg.h \
        KtAlarmClockParam.h \
        KtAlarmClockPlugin.h \
        KtAlarmClockCmd.h \
        KtAlarmClock.h

# DISTFILES = qmldir
#
#!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
#    copy_qmldir.target = $$OUT_PWD/qmldir
#    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
#    copy_qmldir.commands = $(COPY_FILE) "$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)" "$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)"
#    QMAKE_EXTRA_TARGETS += copy_qmldir
#    PRE_TARGETDEPS += $$copy_qmldir.target
#}
#
#qmldir.files = qmldir
#unix {
#    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \., /)
#    qmldir.path = $$installPath
#    target.path = $$installPath
#    INSTALLS += target qmldir
#}

RESOURCES += \
    main.qrc

DISTFILES += \
    KtAlarmTheme.qml \
    KtMouseAreaMove.qml \
    KtSlider.qml \
    KtSpinBox.qml \
    KtTimeSlide.qml \
    KtToolButton.qml \
    KtWindow.qml \
    KtWindowBase.qml \
    KtWindowOver.qml \
    MyClock.qml \
    MyFooter.qml \
    MyLeft.qml \
    MyMain.qml \
    MyOver0.qml \
    MyOver1.qml \
    MyRight.qml \
    MyRightForm.ui.qml \
    MySystemTrayIcon.qml \
    MyWindowMenu.qml \
    main.js \
    MyOver0.js \
    main.qml
