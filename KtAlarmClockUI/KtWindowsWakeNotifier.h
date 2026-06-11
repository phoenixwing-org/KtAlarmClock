/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtWindowsWakeNotifier.h
 * @brief       Windows 电源管理唤醒通知（WM_POWERBROADCAST）
 */
#ifndef KtWindowsWakeNotifier_H
#define KtWindowsWakeNotifier_H

#include <QObject>

/**
 * @brief 监听系统从睡眠/休眠恢复（不依赖 Qt::ApplicationActive）
 */
class KtWindowsWakeNotifier : public QObject {
    Q_OBJECT

public:
    explicit KtWindowsWakeNotifier(QObject* parent = nullptr);
    ~KtWindowsWakeNotifier() override;

signals:
    /** @brief 系统已从睡眠/休眠恢复 */
    void system_resumed();

private:
    class NativeFilter;

    NativeFilter* nativeFilter_; ///< 1. 原生事件过滤器
};

#endif // KtWindowsWakeNotifier_H
