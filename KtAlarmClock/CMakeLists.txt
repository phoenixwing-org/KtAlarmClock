include(../common.cmake)

# 指定工程名
project(KtAlarmClock LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
)

# KtAlarmClock 锁屏功能依赖 Windows API；macOS 下通过全屏遮罩仍可实现锁屏效果
if(NOT WIN32)
    message(STATUS "KtAlarmClock: 非 Windows 平台，锁屏通过全屏遮罩实现")
endif()

# 构建可执行文件
if(WIN32)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_executable(${PROJECT_NAME} main.cpp)
    else()
        add_executable(${PROJECT_NAME} WIN32 main.cpp)
    endif()
    target_sources(${PROJECT_NAME} PRIVATE main.rc)
else()
    # macOS / Linux：无 WIN32 子系统概念，无 .rc 资源文件
    add_executable(${PROJECT_NAME} main.cpp)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE QT_DEPRECATED_WARNINGS)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../KtAlarmClockUI
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    KtAlarmClockUI
    Qt5::Core
    Qt5::Widgets
)
