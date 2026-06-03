include(../common.cmake)
# 指定工程名
project(KtAlarmClock LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Qml
    Quick
)

add_executable(${PROJECT_NAME} main.cpp)

if(WIN32)
    target_sources(${PROJECT_NAME} PRIVATE main.rc)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE QT_DEPRECATED_WARNINGS)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../KtAlarmClockUI
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    KtAlarmClockUI
    Qt5::Core
    Qt5::Qml
    Qt5::Quick
)
