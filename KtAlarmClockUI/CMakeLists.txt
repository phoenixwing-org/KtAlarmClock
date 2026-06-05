include(../common.cmake)
# 指定工程名
project(KtAlarmClockUI LANGUAGES CXX)

# 设置Qt工具链的环境变量
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Qml
    Quick
    Multimedia
    Widgets
    Svg
)

set(SOURCES
    KtAlarmClockCore.cpp
    KtAlarmClockDlg.cpp
    KtAlarmClockParam.cpp
    KtAlarmClockCmd.cpp
    KtAlarmClock.cpp
    KtWallClockEngine.cpp
    KtLockScreenManager.cpp
    KtLockScreenPrimaryWidget.cpp
    KtLockScreenSecondaryWidget.cpp
    KtMainClockWidget.cpp
    KtAlarmClockTray.cpp
    KtAlarmClockController.cpp
    KtAlarmClockSettingWindow.cpp
)

set(HEADERS
    KtAlarmClockCore.h
    KtAlarmClockDlg.h
    KtAlarmClockParam.h
    KtAlarmClockCmd.h
    KtAlarmClock.h
    KtWallClockEngine.h
    KtLockScreenManager.h
    KtLockScreenPrimaryWidget.h
    KtLockScreenSecondaryWidget.h
    KtMainClockWidget.h
    KtAlarmClockTray.h
    KtAlarmClockController.h
    KtAlarmClockSettingWindow.h
)

set(UI_FILES
    KtAlarmClockSettingWindow.ui
)

add_library(${PROJECT_NAME} SHARED
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    KtAlarmClock.qrc
)

target_compile_definitions(${PROJECT_NAME} PRIVATE KtAlarmClockUI_EXPORTS)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${KT_BASE_INCLUDE}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Qml
    Qt5::Quick
    Qt5::Multimedia
    Qt5::Widgets
    Qt5::Svg
)
