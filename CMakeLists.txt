cmake_minimum_required(VERSION 3.25)
project(KtAlarmClockWsp VERSION 1.0.0 LANGUAGES CXX)

set(KT_APP_NAME "KtAlarmClock" CACHE STRING "Application executable and settings name")
set(KT_APP_DISPLAY_NAME "KtAlarmClock" CACHE STRING "User visible application name")
set(KT_COMPANY "KuntaiSoft" CACHE STRING "Company name for metadata and settings")
set(KT_COPYRIGHT "Copyright (c) 2026 KuntaiSoft. All rights reserved." CACHE STRING "Application copyright")
set(KT_DOMAIN "com.kuntaisoft.KtAlarmClock" CACHE STRING "Application domain identifier")
set(KT_DESCRIPTION "Screen alarm clock with Pomodoro rhythm" CACHE STRING "Application description")
set(KT_PROJECT_VERSION "${PROJECT_VERSION}" CACHE STRING "Application version")
set(KT_QT_MIN_VERSION 6.2 CACHE STRING "Minimum supported Qt version")
option(KT_STATIC_BUILD "Build with static app libraries and static Qt packaging assumptions" OFF)

if(KT_QT_MIN_VERSION VERSION_LESS 6.0)
    message(FATAL_ERROR "KtAlarmClock requires Qt 6 or newer.")
endif()

find_package(Qt6 ${KT_QT_MIN_VERSION} REQUIRED COMPONENTS Core)
message(STATUS "Qt version: ${Qt6_VERSION}")

find_package(Git QUIET)
if(NOT DEFINED KT_PROJECT_BRANCH OR KT_PROJECT_BRANCH STREQUAL "")
    if(Git_FOUND)
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE KT_GIT_BRANCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    endif()

    if(KT_GIT_BRANCH STREQUAL "release")
        set(KT_PROJECT_BRANCH "release" CACHE STRING "Project branch mode" FORCE)
    else()
        set(KT_PROJECT_BRANCH "debug" CACHE STRING "Project branch mode" FORCE)
    endif()
endif()

message(STATUS "KtAlarmClock branch: ${KT_PROJECT_BRANCH}")
message(STATUS "KtAlarmClock static build: ${KT_STATIC_BUILD}")
message(STATUS "KtAlarmClock version: ${KT_PROJECT_VERSION}")

enable_testing()

add_subdirectory(KtAlarmClockUI)
add_subdirectory(KtAlarmClock)
