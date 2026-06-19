/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtSingleInstanceGuard.cpp
 */
#include "KtSingleInstanceGuard.h"

#include "KtAlarmClockParam.h"

#include <QDebug>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>

namespace {
constexpr int kConnectTimeoutMs = 400;
constexpr auto kActivateToken   = "activate";
} // namespace

//------------------------------------------------------
KtSingleInstanceGuard::KtSingleInstanceGuard(QObject* parent)
    : QObject(parent)
    , server_(nullptr) { // 1
}
//------------------------------------------------------
KtSingleInstanceGuard::~KtSingleInstanceGuard() {
    if (server_) {
        server_->close();
        server_ = nullptr; // 1
    }
}
//------------------------------------------------------
void KtSingleInstanceGuard::on_new_connection() {
    if (!server_)
        return;

    QLocalSocket* client = server_->nextPendingConnection();
    if (!client)
        return;

    const auto handleActivate = [this, client]() {
        if (client->readAll().contains(kActivateToken))
            emit activate_requested();
        client->disconnectFromServer();
    };
    connect(client, &QLocalSocket::readyRead, this, handleActivate);
    connect(client, &QLocalSocket::disconnected, client, &QLocalSocket::deleteLater);
    if (client->bytesAvailable() > 0)
        handleActivate();
}
//------------------------------------------------------
QString KtSingleInstanceGuard::server_name() {
    return QStringLiteral("KtAlarmClock_%1").arg(QLatin1String(VERSION_KtAlarmClock));
}
//------------------------------------------------------
bool KtSingleInstanceGuard::start_listening() {
    if (server_)
        return true;

    server_ = new QLocalServer(this);
    connect(server_, &QLocalServer::newConnection, this,
            &KtSingleInstanceGuard::on_new_connection);

    QLocalServer::removeServer(server_name()); // 清理异常退出残留
    if (!server_->listen(server_name())) {
        qWarning() << "[SingleInstance] listen failed:" << server_->errorString();
        return false;
    }

    return true;
}
//------------------------------------------------------
bool KtSingleInstanceGuard::try_acquire_primary() {
    QLocalSocket socket;
    socket.connectToServer(server_name());
    if (!socket.waitForConnected(kConnectTimeoutMs))
        return true; // 无已运行实例

    socket.write(kActivateToken);
    socket.flush();
    socket.waitForBytesWritten(kConnectTimeoutMs);
    socket.disconnectFromServer();

    QMessageBox::information(
        nullptr,
        QStringLiteral("KT护眼闹钟"),
        QStringLiteral("程序已在运行中，已通知已有窗口显示。"));

    return false;
}
