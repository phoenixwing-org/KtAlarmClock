cmake_minimum_required (VERSION 3.25)
project(KtAlarmClockWsp VERSION 1.0.0)

include(common.cmake)

enable_testing()

# Custom Widget
add_subdirectory (KtAlarmClockUI)
add_subdirectory (KtAlarmClock)
