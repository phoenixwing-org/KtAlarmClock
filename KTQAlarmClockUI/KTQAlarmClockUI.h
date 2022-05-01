/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2021
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockUI.h
 * @version		V1.0
 * @brief       Shared export macro define
 * @details     KTQAlarmClockUI_EXPORTS is defined in MSVC or CMakeLists.txt file.
 * ExportedByKTQAlarmClockUI is used for windows lib
 */
#ifndef _KTQAlarmClockUI_EXPORT_H_
#define _KTQAlarmClockUI_EXPORT_H_
#ifdef _WIN32
#if defined(KTQAlarmClockUI_EXPORTS)
#define ExportedByKTQAlarmClockUI __declspec(dllexport)
#else
#define ExportedByKTQAlarmClockUI __declspec(dllimport)
#endif
#elif __linux__
#define ExportedByKTQAlarmClockUI
#else
#error "Unknown compiler"
#endif

#endif // _KTQAlarmClockUI_EXPORT_H_
