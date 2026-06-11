include(../common.cmake)
# 指定工程名
project(KtAlarmClock LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
)

# 改为不支持非 win32 平台，MAC 下锁屏锁不住
if(NOT WIN32)
    message(FATAL_ERROR "KtAlarmClock does not support non-Windows platforms")
endif()

# Debug：CONSOLE 子系统，保留终端黑窗口便于 qDebug；Release：WIN32 纯 GUI
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_executable(${PROJECT_NAME} main.cpp)
else()
    add_executable(${PROJECT_NAME} WIN32 main.cpp)
endif()

target_sources(${PROJECT_NAME} PRIVATE main.rc)

target_compile_definitions(${PROJECT_NAME} PRIVATE QT_DEPRECATED_WARNINGS)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../KtAlarmClockUI
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    KtAlarmClockUI
    Qt5::Core
    Qt5::Widgets
)
