include(../common.cmake)
# 指定工程名
project(KtAlarmClockUI LANGUAGES CXX)

# 设置Qt工具链的环境变量
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Qml
    Quick
    Multimedia
)

set(SOURCES
    KtAlarmClockCore.cpp
    KtAlarmClockDlg.cpp
    KtAlarmClockParam.cpp
    KtAlarmClockPlugin.cpp
    KtAlarmClockCmd.cpp
    KtAlarmClock.cpp
)

set(HEADERS
    KtAlarmClockCore.h
    KtAlarmClockDlg.h
    KtAlarmClockParam.h
    KtAlarmClockPlugin.h
    KtAlarmClockCmd.h
    KtAlarmClock.h
)

add_library(${PROJECT_NAME} SHARED
    ${SOURCES}
    ${HEADERS}
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
)
