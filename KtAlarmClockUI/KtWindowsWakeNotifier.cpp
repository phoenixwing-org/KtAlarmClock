/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     Apache-2.0
 * @author      Kuntai
 * @file        KtWindowsWakeNotifier.cpp
 */
#include "KtWindowsWakeNotifier.h"

#include <QAbstractNativeEventFilter>
#include <QCoreApplication>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

//------------------------------------------------------
class KtWindowsWakeNotifier::NativeFilter : public QAbstractNativeEventFilter {
public:
    explicit NativeFilter(KtWindowsWakeNotifier* owner)
        : owner_(owner) { // 1
    }

    bool nativeEventFilter(const QByteArray& eventType, void* message,
                           long* result) override {
        Q_UNUSED(result);

#ifdef _WIN32
        if (eventType != "windows_generic_MSG" || !owner_)
            return false;

        const auto* msg = static_cast<MSG*>(message);
        if (msg->message != WM_POWERBROADCAST)
            return false;

        switch (msg->wParam) {
        case PBT_APMRESUMEAUTOMATIC: // 自动唤醒（合盖开盖等）
        case PBT_APMRESUMESUSPEND:   // 从挂起恢复
        case PBT_APMRESUMECRITICAL:  // 从关键挂起恢复
            QMetaObject::invokeMethod(owner_, "system_resumed", Qt::QueuedConnection);
            break;
        default:
            break;
        }
#endif
        return false;
    }

private:
    KtWindowsWakeNotifier* owner_; ///< 1
};

//------------------------------------------------------
KtWindowsWakeNotifier::KtWindowsWakeNotifier(QObject* parent)
    : QObject(parent)
    , nativeFilter_(nullptr) { // 1
#ifdef _WIN32
    nativeFilter_ = new NativeFilter(this);
    if (auto* app = QCoreApplication::instance())
        app->installNativeEventFilter(nativeFilter_);
#endif
}
//------------------------------------------------------
KtWindowsWakeNotifier::~KtWindowsWakeNotifier() {
#ifdef _WIN32
    if (nativeFilter_) {
        if (auto* app = QCoreApplication::instance())
            app->removeNativeEventFilter(nativeFilter_);
        delete nativeFilter_;
        nativeFilter_ = nullptr; // 1
    }
#endif
}
