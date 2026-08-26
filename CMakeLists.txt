cmake_minimum_required (VERSION 3.25)
project(KtAlarmClockWsp VERSION 1.0.0)

include("$ENV{ROOT_DIR}/cmake/SdkCore.cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
sdk_enable_global_cxx_utf8()
sdk_set_global_config_output_dirs("${SDK_OUTPUT_ROOT}")

sdk_enable_ctest()

# Custom Widget
add_subdirectory (KtAlarmClockUI)
add_subdirectory (KtAlarmClock)
