include(../common.cmake)
project(KtAlarmClockUI VERSION ${KT_PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 ${KT_QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    LinguistTools
    Qml
    Quick
    QuickControls2
    Widgets
    Svg
)

set(SOURCES
    src/source/KtAlarmClockParam.cpp
    src/source/KtAlarmClockSettings.cpp
    src/source/KtAlarmClockCmd.cpp
    src/source/KtAlarmClock.cpp
    src/source/KtAlarmClockQmlShell.cpp
    src/source/KtAlarmClockViewModel.cpp
    src/source/KtAudioMuteGuard.cpp
    src/source/KtClockRuntime.cpp
    src/source/KtKeyboardBlocker.cpp
    src/source/KtQpcClock.cpp
    src/source/KtMonotonicCountdown.cpp
    src/source/KtLockScreenManager.cpp
    src/source/KtTrayService.cpp
    src/source/KtTranslationManager.cpp
    src/source/KtAlarmClockController.cpp
    src/source/KtScreenUtil.cpp
)

set(HEADERS
    src/header/KtAlarmClockParam.h
    src/header/KtAlarmClockSettings.h
    src/header/KtAlarmClockCmd.h
    src/header/KtAlarmClock.h
    src/header/KtAlarmClockQmlShell.h
    src/header/KtAlarmClockViewModel.h
    src/header/KtAudioMuteGuard.h
    src/header/KtClockRuntime.h
    src/header/KtKeyboardBlocker.h
    src/header/KtPropertyMacros.h
    src/header/KtQpcClock.h
    src/header/KtMonotonicCountdown.h
    src/header/KtLockScreenManager.h
    src/header/KtTrayService.h
    src/header/KtTranslationManager.h
    src/header/KtAlarmClockController.h
    src/header/KtAlarmClockUI.h
    src/header/KtScreenUtil.h
)

set(IMAGE_RESOURCES
    res/image/coffee.svg
    res/image/kt-blue.svg
)

set(FONT_RESOURCES
    res/font/codicon-font-v0.0.37-1.ttf
)

set(TRANSLATION_TS_FILES
    res/linguist/KtAlarmClock_en_US.ts
    res/linguist/KtAlarmClock_zh_CN.ts
)

set(TRANSLATION_QM_FILES
    res/linguist/KtAlarmClock_en_US.qm
    res/linguist/KtAlarmClock_zh_CN.qm
)

set(QML_FILES
    res/qml/Main.qml
    res/qml/BubbleContextMenu.qml
    res/qml/ClockBubble.qml
    res/qml/SettingsPanel.qml
    res/qml/DurationField.qml
    res/qml/SlideUnlock.qml
    res/qml/PinCodeInput.qml
    res/qml/LockPrimary.qml
    res/qml/LockSecondary.qml
    res/qml/FormulaUnlock.qml
)

set(KT_UI_LIBRARY_TYPE SHARED)
if(KT_STATIC_BUILD)
    set(KT_UI_LIBRARY_TYPE STATIC)
endif()

add_library(${PROJECT_NAME} ${KT_UI_LIBRARY_TYPE}
    ${SOURCES}
    ${HEADERS}
)

kt_apply_common_target_settings(${PROJECT_NAME})

qt_add_qml_module(${PROJECT_NAME}
    URI KtAlarmClockQml
    VERSION 1.0
    RESOURCE_PREFIX /qt/qml
    OUTPUT_DIRECTORY "${KT_QML_IMPORT_OUTPUT_DIR}/KtAlarmClockQml"
    QML_FILES ${QML_FILES}
)

if(TARGET KtAlarmClockUIplugin)
    kt_apply_output_directories(KtAlarmClockUIplugin)
endif()

qt_add_resources(${PROJECT_NAME} "KtAlarmClockImages"
    PREFIX "/image"
    BASE "res/image"
    FILES ${IMAGE_RESOURCES}
)

qt_add_resources(${PROJECT_NAME} "KtAlarmClockFonts"
    PREFIX "/font"
    BASE "res/font"
    FILES ${FONT_RESOURCES}
)

qt_add_resources(${PROJECT_NAME} "KtAlarmClockTranslations"
    PREFIX "/i18n"
    BASE "res/linguist"
    FILES ${TRANSLATION_QM_FILES}
)

if(KT_STATIC_BUILD)
    target_compile_definitions(${PROJECT_NAME} PUBLIC KtAlarmClockUI_STATIC)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE KtAlarmClockUI_EXPORTS)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/header
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Widgets
    Qt6::Svg
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE Ole32 Uuid)
endif()

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    kt_find_qt_tool(QT_LUPDATE_EXECUTABLE lupdate lupdate-qt6)
    kt_find_qt_tool(QT_LRELEASE_EXECUTABLE lrelease lrelease-qt6)

    if(QT_LUPDATE_EXECUTABLE AND QT_LRELEASE_EXECUTABLE)
        set(KT_TRANSLATION_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/src/header"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/source"
            "${CMAKE_CURRENT_SOURCE_DIR}/res/qml"
        )

        add_custom_target(${PROJECT_NAME}_translations
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/res/linguist"
            COMMAND "${QT_LUPDATE_EXECUTABLE}" ${KT_TRANSLATION_SOURCES}
                -source-language zh_CN
                -ts "${CMAKE_CURRENT_SOURCE_DIR}/res/linguist/KtAlarmClock_en_US.ts"
            COMMAND "${QT_LUPDATE_EXECUTABLE}" ${KT_TRANSLATION_SOURCES}
                -source-language zh_CN
                -ts "${CMAKE_CURRENT_SOURCE_DIR}/res/linguist/KtAlarmClock_zh_CN.ts"
            COMMAND "${QT_LRELEASE_EXECUTABLE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/res/linguist/KtAlarmClock_en_US.ts"
            COMMAND "${QT_LRELEASE_EXECUTABLE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/res/linguist/KtAlarmClock_zh_CN.ts"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            SOURCES ${TRANSLATION_TS_FILES}
            COMMENT "Updating KtAlarmClock translations"
            VERBATIM
        )
        add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_translations)
    else()
        message(WARNING "Qt Linguist tools were not found. Translation files will not be regenerated.")
    endif()
endif()

add_executable(KtAlarmClockCoreTests
    tests/KtAlarmClockCoreTests.cpp
)

kt_apply_common_target_settings(KtAlarmClockCoreTests)

target_link_libraries(KtAlarmClockCoreTests PRIVATE
    KtAlarmClockUI
    Qt6::Core
)

add_test(NAME KtAlarmClockCoreTests COMMAND $<TARGET_FILE:KtAlarmClockCoreTests>)
set(KT_ALARM_CLOCK_CORE_TEST_PATH_MODS
    "PATH=path_list_prepend:$<TARGET_FILE_DIR:KtAlarmClockUI>"
    "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
set_tests_properties(KtAlarmClockCoreTests PROPERTIES
    ENVIRONMENT_MODIFICATION "${KT_ALARM_CLOCK_CORE_TEST_PATH_MODS}"
)
