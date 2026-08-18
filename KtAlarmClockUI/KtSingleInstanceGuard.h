/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     Apache-2.0
 * @author      Kuntai
 * @file        KtSingleInstanceGuard.h
 * @brief       单实例启动守卫（QLocalServer / QLocalSocket）
 */
#ifndef KtSingleInstanceGuard_H
#define KtSingleInstanceGuard_H

#include <QObject>

class QLocalServer;

/**
 * @brief 保证进程唯一；二次启动时通知主实例并提示后退出
 */
class KtSingleInstanceGuard : public QObject {
    Q_OBJECT

public:
    explicit KtSingleInstanceGuard(QObject* parent = nullptr);
    ~KtSingleInstanceGuard() override;

public:
    /** @brief 主实例开始监听二次启动请求 */
    bool start_listening();

    /**
     * @brief 尝试成为主实例
     * @return 若已有实例在运行则弹窗提示、发送激活信号并返回 false
     */
    bool try_acquire_primary();

private:
    void on_new_connection();

    /** @brief 本地套接字服务名（含版本号，避免跨版本互抢） */
    static QString server_name();

signals:
    /** @brief 收到二次启动的「显示」请求 */
    void activate_requested();

private:
    QLocalServer* server_; ///< 1. 主实例监听器
};

#endif // KtSingleInstanceGuard_H
