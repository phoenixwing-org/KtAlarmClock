#used for several cmake file
cmake_minimum_required(VERSION 3.25)

# 设置C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

# Read env paths and normalize separators (\ -> /) in one step per variable
file(TO_CMAKE_PATH "$ENV{ROOT_DIR}" ROOT_DIR)
file(TO_CMAKE_PATH "$ENV{ROOT_DIR}/kt/viewer" ROOT_DIR_VIEWER)
file(TO_CMAKE_PATH "$ENV{ROOT_DIR_3rdParty}" ROOT_DIR_3rdParty)

# set default for output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${ROOT_DIR_VIEWER}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${ROOT_DIR_VIEWER}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ROOT_DIR_VIEWER}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ROOT_DIR_VIEWER}/bin)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${ROOT_DIR_VIEWER}/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${ROOT_DIR_VIEWER}/bin)

# 根据构建类型设置CMAKE_BUILD_TYPE
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
else()
    set(CMAKE_BUILD_TYPE Release)
endif()
