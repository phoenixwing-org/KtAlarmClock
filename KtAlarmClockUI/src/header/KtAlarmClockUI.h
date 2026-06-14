#ifndef _KtAlarmClockUI_EXPORT_H_
#define _KtAlarmClockUI_EXPORT_H_

#ifdef _WIN32
#if defined(KtAlarmClockUI_STATIC)
#define ExportedByKtAlarmClockUI
#elif defined(KtAlarmClockUI_EXPORTS)
#define ExportedByKtAlarmClockUI __declspec(dllexport)
#else
#define ExportedByKtAlarmClockUI __declspec(dllimport)
#endif
#elif __linux__
#define ExportedByKtAlarmClockUI
#else
#error "Unknown compiler"
#endif

#define KT_S_OK 0
#define KT_E_INVALIDARG 100002

#endif // _KtAlarmClockUI_EXPORT_H_
