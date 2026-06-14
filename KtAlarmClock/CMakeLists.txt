include(../common.cmake)
project(KtAlarmClock VERSION ${KT_PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 ${KT_QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Qml
    Quick
    QuickControls2
    Svg
    Widgets
)

set(SOURCES
    src/source/main.cpp
)

set(RESOURCE_FILES)

if(WIN32)
    set(KT_APP_ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/res/image/kt.ico")
    set(KT_VERSION_RC "${PROJECT_VERSION_MAJOR},${PROJECT_VERSION_MINOR},${PROJECT_VERSION_PATCH},0")
    configure_file(.cmake/win_app.rc.in ${CMAKE_CURRENT_BINARY_DIR}/win_app.rc @ONLY)
    configure_file(.cmake/qt.conf.in ${CMAKE_CURRENT_BINARY_DIR}/qt.conf @ONLY)
    list(APPEND RESOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/win_app.rc)
endif()

qt_add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${RESOURCE_FILES}
)

if(KT_STATIC_BUILD)
    qt_add_qml_module(${PROJECT_NAME}
        URI KtAlarmClockStaticImports
        VERSION 1.0
        RESOURCE_PREFIX /qt/qml
        QML_FILES StaticImports.qml
    )
endif()

kt_apply_common_target_settings(${PROJECT_NAME})

if(MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE /MANIFEST:NO)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    KtAlarmClockUI
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Svg
    Qt6::Widgets
)

set(KT_APP_DEPLOY_DEPENDENCIES ${PROJECT_NAME} KtAlarmClockUI)
if(TARGET KtAlarmClockUIplugin)
    list(APPEND KT_APP_DEPLOY_DEPENDENCIES KtAlarmClockUIplugin)
    add_dependencies(${PROJECT_NAME} KtAlarmClockUIplugin)
    if(KT_STATIC_BUILD)
        get_target_property(KT_UI_PLUGIN_TYPE KtAlarmClockUIplugin TYPE)
        if(NOT KT_UI_PLUGIN_TYPE STREQUAL "STATIC_LIBRARY")
            message(FATAL_ERROR "KT_STATIC_BUILD requires a static Qt build; KtAlarmClockUIplugin is ${KT_UI_PLUGIN_TYPE}. Check QT_STATIC_ROOT_MANUAL in build.bat.")
        endif()
        target_link_libraries(${PROJECT_NAME} PRIVATE KtAlarmClockUIplugin)
    endif()
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME "${KT_APP_NAME}"
)

set(KT_ENABLE_CONSOLE_SUBSYSTEM_DEFAULT OFF)
if(KT_PROJECT_BRANCH STREQUAL "debug")
    set(KT_ENABLE_CONSOLE_SUBSYSTEM_DEFAULT ON)
endif()
option(KT_ENABLE_CONSOLE_SUBSYSTEM "Build the app executable with the Windows console subsystem" ${KT_ENABLE_CONSOLE_SUBSYSTEM_DEFAULT})
if(NOT DEFINED KT_DIST_SUBSYSTEM OR KT_DIST_SUBSYSTEM STREQUAL "" OR KT_DIST_SUBSYSTEM STREQUAL "AUTO")
    set(KT_DIST_SUBSYSTEM "WINDOWS" CACHE STRING "Windows subsystem for the copied dist executable: CONSOLE or WINDOWS" FORCE)
else()
    set(KT_DIST_SUBSYSTEM "${KT_DIST_SUBSYSTEM}" CACHE STRING "Windows subsystem for the copied dist executable: CONSOLE or WINDOWS")
endif()
set_property(CACHE KT_DIST_SUBSYSTEM PROPERTY STRINGS CONSOLE WINDOWS)

if(MSVC AND KT_ENABLE_CONSOLE_SUBSYSTEM)
    target_link_options(${PROJECT_NAME} PRIVATE /SUBSYSTEM:CONSOLE)
endif()

option(KT_ENABLE_WINDEPLOYQT "Run windeployqt after building the Windows app" ON)
option(KT_DIST_CLEANUP "Remove unused files from the distribution directory after windeployqt" ON)
option(KT_DIST_STRIP_OWN_QML_SOURCES "Remove the app's own QML source files from the distribution module" ON)
set(KT_DEFAULT_DIST_PATH "${KT_VIEWER_DIST_DIR}")
set(KT_LEGACY_DIST_PATH "${CMAKE_BINARY_DIR}/${KT_APP_NAME}_DIST")
if(NOT DEFINED KT_DIST_PATH OR KT_DIST_PATH STREQUAL "" OR KT_DIST_PATH STREQUAL "${KT_LEGACY_DIST_PATH}")
    set(KT_DIST_PATH "${KT_DEFAULT_DIST_PATH}" CACHE PATH "Distribution output directory" FORCE)
else()
    set(KT_DIST_PATH "${KT_DIST_PATH}" CACHE PATH "Distribution output directory")
endif()

if(KT_STATIC_BUILD)
    qt_import_qml_plugins(${PROJECT_NAME})
    qt_import_plugins(${PROJECT_NAME})
endif()

if(WIN32 AND KT_STATIC_BUILD)
    add_custom_target(${PROJECT_NAME}_dist
        COMMAND "${CMAKE_COMMAND}" -E rm -rf "${KT_DIST_PATH}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${KT_DIST_PATH}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_FILE:${PROJECT_NAME}>"
            "${KT_DIST_PATH}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        DEPENDS ${PROJECT_NAME}
        COMMENT "Creating static ${PROJECT_NAME} distribution in ${KT_DIST_PATH}"
        VERBATIM
    )
elseif(WIN32 AND KT_ENABLE_WINDEPLOYQT)
    kt_find_qt_tool(WINDEPLOYQT_EXECUTABLE windeployqt)

    if(WINDEPLOYQT_EXECUTABLE)
        set(KT_WINDEPLOYQT_MODE_ARGS
            "$<$<CONFIG:Debug>:--debug>"
            "$<$<NOT:$<CONFIG:Debug>>:--release>"
        )

        set(KT_RUNTIME_QML_MODULE_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>/qml/KtAlarmClockQml")
        set(KT_DIST_QML_MODULE_DIR "${KT_DIST_PATH}/qml/KtAlarmClockQml")
        set(KT_DIST_CLEANUP_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/dist_cleanup.cmake")
        configure_file(.cmake/dist_cleanup.cmake.in "${KT_DIST_CLEANUP_SCRIPT}" @ONLY)
        set(KT_COPY_RUNTIME_QML_PLUGIN_COMMANDS)
        set(KT_COPY_DIST_QML_PLUGIN_COMMANDS)
        set(KT_DIST_CLEANUP_COMMANDS)
        set(KT_DIST_SUBSYSTEM_COMMANDS)

        if(KT_DIST_CLEANUP)
            list(APPEND KT_DIST_CLEANUP_COMMANDS
                COMMAND "${CMAKE_COMMAND}" -P "${KT_DIST_CLEANUP_SCRIPT}"
            )
        endif()

        if(MSVC)
            string(TOUPPER "${KT_DIST_SUBSYSTEM}" KT_RESOLVED_DIST_SUBSYSTEM)
            if(NOT KT_RESOLVED_DIST_SUBSYSTEM STREQUAL "CONSOLE" AND NOT KT_RESOLVED_DIST_SUBSYSTEM STREQUAL "WINDOWS")
                message(FATAL_ERROR "KT_DIST_SUBSYSTEM must be CONSOLE or WINDOWS.")
            endif()

            get_filename_component(KT_MSVC_BIN_DIR "${CMAKE_LINKER}" DIRECTORY)
            find_program(EDITBIN_EXECUTABLE editbin HINTS "${KT_MSVC_BIN_DIR}" NO_CACHE)
            if(EDITBIN_EXECUTABLE)
                list(APPEND KT_DIST_SUBSYSTEM_COMMANDS
                    COMMAND "${EDITBIN_EXECUTABLE}"
                        "/SUBSYSTEM:${KT_RESOLVED_DIST_SUBSYSTEM}"
                        "${KT_DIST_PATH}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
                )
            else()
                message(WARNING "editbin was not found. KT_DIST_SUBSYSTEM cannot be applied to the dist executable.")
            endif()
        endif()

        if(TARGET KtAlarmClockUIplugin)
            list(APPEND KT_COPY_RUNTIME_QML_PLUGIN_COMMANDS
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "$<TARGET_FILE:KtAlarmClockUIplugin>"
                    "${KT_RUNTIME_QML_MODULE_DIR}/$<TARGET_FILE_NAME:KtAlarmClockUIplugin>"
            )
            list(APPEND KT_COPY_DIST_QML_PLUGIN_COMMANDS
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "$<TARGET_FILE:KtAlarmClockUIplugin>"
                    "${KT_DIST_PATH}/$<TARGET_FILE_NAME:KtAlarmClockUIplugin>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "$<TARGET_FILE:KtAlarmClockUIplugin>"
                    "${KT_DIST_QML_MODULE_DIR}/$<TARGET_FILE_NAME:KtAlarmClockUIplugin>"
            )
        endif()

        add_custom_target(${PROJECT_NAME}_deploy ALL
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/qt.conf"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${KT_RUNTIME_QML_MODULE_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${KT_QML_IMPORT_OUTPUT_DIR}/KtAlarmClockQml"
                "${KT_RUNTIME_QML_MODULE_DIR}"
            ${KT_COPY_RUNTIME_QML_PLUGIN_COMMANDS}
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                ${KT_WINDEPLOYQT_MODE_ARGS}
                --no-translations
                --qmldir "${CMAKE_SOURCE_DIR}/KtAlarmClockUI/res/qml"
                --dir "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
                "$<TARGET_FILE:${PROJECT_NAME}>"
            COMMAND_EXPAND_LISTS
            DEPENDS ${KT_APP_DEPLOY_DEPENDENCIES}
            COMMENT "Deploying Qt runtime for ${PROJECT_NAME}"
            VERBATIM
        )

        add_custom_target(${PROJECT_NAME}_dist
            COMMAND "${CMAKE_COMMAND}" -E rm -rf "${KT_DIST_PATH}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${KT_DIST_PATH}"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:${PROJECT_NAME}>"
                "${KT_DIST_PATH}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
            ${KT_DIST_SUBSYSTEM_COMMANDS}
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:KtAlarmClockUI>"
                "${KT_DIST_PATH}/$<TARGET_FILE_NAME:KtAlarmClockUI>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
                "${KT_DIST_PATH}/qt.conf"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${KT_DIST_QML_MODULE_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${KT_QML_IMPORT_OUTPUT_DIR}/KtAlarmClockQml"
                "${KT_DIST_QML_MODULE_DIR}"
            ${KT_COPY_DIST_QML_PLUGIN_COMMANDS}
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                ${KT_WINDEPLOYQT_MODE_ARGS}
                --no-translations
                --qmldir "${CMAKE_SOURCE_DIR}/KtAlarmClockUI/res/qml"
                --dir "${KT_DIST_PATH}"
                "${KT_DIST_PATH}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
            ${KT_DIST_CLEANUP_COMMANDS}
            COMMAND_EXPAND_LISTS
            DEPENDS ${KT_APP_DEPLOY_DEPENDENCIES}
            COMMENT "Creating ${PROJECT_NAME} distribution in ${KT_DIST_PATH}"
            VERBATIM
        )
    else()
        message(WARNING "windeployqt was not found. ${PROJECT_NAME} may fail to run outside a Qt build environment.")
    endif()
endif()
