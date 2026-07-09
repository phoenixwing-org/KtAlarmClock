# 指定工程名
project(KtAlarmClockUI LANGUAGES CXX)

# 设置Qt工具链的环境变量
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network Svg)

file(GLOB_RECURSE PROJECT_SOURCES "*.cpp" "*.h" "*.ui" "*.qrc" "public/*.h" )
 
add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE KtAlarmClockUI_EXPORTS)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${KT_BASE_INCLUDE}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core Qt5::Widgets Qt5::Network Qt5::Svg
)
