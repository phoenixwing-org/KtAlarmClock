/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2021
 * @license     Apache-2.0
 * @author      Kuntai
 * @file        KtAlarmClockUI.h
 * @version		V1.0
 * @brief       Shared export macro define
 * @details     KtAlarmClockUI_EXPORTS is defined in MSVC or CMakeLists.txt file.
 * ExportedByKtAlarmClockUI is used for windows lib
 */
#ifndef _KtAlarmClockUI_EXPORT_H_
#define _KtAlarmClockUI_EXPORT_H_
#ifdef _WIN32
#if defined(KtAlarmClockUI_EXPORTS)
#define ExportedByKtAlarmClockUI __declspec(dllexport)
#else
#define ExportedByKtAlarmClockUI __declspec(dllimport)
#endif
#elif defined(__linux__) || defined(__APPLE__)
#define ExportedByKtAlarmClockUI
#else
#error "Unknown compiler"
#endif

#define KT_S_OK              0
#define KT_S_ALREADY_RUNNING 1
#define KT_E_FAIL 100001
#define KT_E_INVALIDARG 100002
#define KT_E_UNEXPECTED 100003

#define KTDelete(ptr)  \
    if (ptr) {         \
        delete ptr;    \
        ptr = nullptr; \
    }
#define KTSetNULL(ptr) \
    { ptr = nullptr; }

#
#endif // _KtAlarmClockUI_EXPORT_H_
