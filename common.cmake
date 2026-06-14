cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(KT_ENABLE_QML_DEBUG "Enable Qt/QML debugger in debug-like builds" OFF)

if(DEFINED ENV{ROOT_DIR} AND NOT "$ENV{ROOT_DIR}" STREQUAL "")
    file(TO_CMAKE_PATH "$ENV{ROOT_DIR}" ROOT_DIR)
else()
    set(ROOT_DIR "${CMAKE_SOURCE_DIR}")
endif()

set(QML_IMPORT_PATH "${CMAKE_BINARY_DIR}/qt/qml" CACHE STRING "Qt/QML import path" FORCE)

file(TO_CMAKE_PATH "${ROOT_DIR}/kt/viewer" KT_VIEWER_ROOT)
set(KT_VIEWER_DEBUG_DIR "${KT_VIEWER_ROOT}/debug")
set(KT_VIEWER_RELEASE_DIR "${KT_VIEWER_ROOT}/bin")
set(KT_VIEWER_LIBRARY_DIR "${KT_VIEWER_ROOT}/lib")
set(KT_VIEWER_DIST_DIR "${KT_VIEWER_ROOT}/dist")
set(KT_QML_IMPORT_OUTPUT_DIR "${CMAKE_BINARY_DIR}/qt/qml")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${KT_VIEWER_LIBRARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${KT_VIEWER_RELEASE_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${KT_VIEWER_RELEASE_DIR}")

foreach(_kt_config DEBUG)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_kt_config} "${KT_VIEWER_LIBRARY_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_kt_config} "${KT_VIEWER_DEBUG_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_kt_config} "${KT_VIEWER_DEBUG_DIR}")
endforeach()

foreach(_kt_config RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_kt_config} "${KT_VIEWER_LIBRARY_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_kt_config} "${KT_VIEWER_RELEASE_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_kt_config} "${KT_VIEWER_RELEASE_DIR}")
endforeach()

if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

function(kt_apply_output_directories target_name)
    set_target_properties(${target_name} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${KT_VIEWER_LIBRARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${KT_VIEWER_RELEASE_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${KT_VIEWER_RELEASE_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${KT_VIEWER_LIBRARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${KT_VIEWER_DEBUG_DIR}"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${KT_VIEWER_DEBUG_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${KT_VIEWER_LIBRARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${KT_VIEWER_RELEASE_DIR}"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${KT_VIEWER_RELEASE_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${KT_VIEWER_LIBRARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${KT_VIEWER_RELEASE_DIR}"
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${KT_VIEWER_RELEASE_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${KT_VIEWER_LIBRARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${KT_VIEWER_RELEASE_DIR}"
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${KT_VIEWER_RELEASE_DIR}"
    )
endfunction()

function(kt_apply_common_target_settings target_name)
    target_compile_features(${target_name} PUBLIC cxx_std_20)
    target_compile_definitions(${target_name} PRIVATE
        QT_MESSAGELOGCONTEXT
        PROJECT_NAME="${PROJECT_NAME}"
        PROJECT_APP_NAME="${KT_APP_NAME}"
        PROJECT_APP_DISPLAY_NAME="${KT_APP_DISPLAY_NAME}"
        PROJECT_COMPANY="${KT_COMPANY}"
        PROJECT_COPYRIGHT="${KT_COPYRIGHT}"
        PROJECT_DOMAIN="${KT_DOMAIN}"
        PROJECT_DESCRIPTION="${KT_DESCRIPTION}"
        PROJECT_VERSION="${KT_PROJECT_VERSION}"
        PROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        PROJECT_BRANCH="${KT_PROJECT_BRANCH}"
        QT_DISABLE_DEPRECATED_UP_TO=0x060000
    )

    if(WIN32)
        target_compile_definitions(${target_name} PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
    endif()

    if(KT_ENABLE_QML_DEBUG)
        target_compile_definitions(${target_name} PRIVATE
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
        )
    else()
        target_compile_definitions(${target_name} PRIVATE QT_NO_QML_DEBUGGER)
    endif()

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /utf-8 /EHsc /permissive- /Zc:__cplusplus /MP)
    endif()

    kt_apply_output_directories(${target_name})
endfunction()

function(kt_resolve_qt_bin_dir output_variable)
    get_target_property(_kt_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    if(_kt_qmake_executable)
        get_filename_component(_kt_qt_bin_dir "${_kt_qmake_executable}" DIRECTORY)
    elseif(DEFINED Qt6_DIR)
        set(_kt_qt_bin_dir "${Qt6_DIR}/../../..")
        cmake_path(SET _kt_qt_bin_dir NORMALIZE "${_kt_qt_bin_dir}/bin")
    else()
        set(_kt_qt_bin_dir "")
    endif()
    set(${output_variable} "${_kt_qt_bin_dir}" PARENT_SCOPE)
endfunction()

function(kt_find_qt_tool output_variable)
    kt_resolve_qt_bin_dir(_kt_qt_bin_dir)
    find_program(_kt_tool
        NAMES ${ARGN}
        HINTS "${_kt_qt_bin_dir}"
        NO_CACHE
    )
    set(${output_variable} "${_kt_tool}" PARENT_SCOPE)
endfunction()
