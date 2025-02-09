#include "Client/RpcClient.h"
#include "msg.hpp"
#include <QDebug>

RpcClient::RpcClient(QObject *parent)
    : Client(parent), client(nullptr) {}

RpcClient::~RpcClient() {
    delete client;
}

void RpcClient::connectToServer(const QString &address, quint16 port) {
    try {
        client = new rpc::client(address.toStdString(), port);
        client->set_timeout(5000); // 强制设置超时
        emit connected(); // 确保此行执行
        qDebug() << "Connected to server at" << address << ":" << port;
    } catch (const std::exception &e) {
        // 检查是否有异常被捕获
        qWarning() << "Connection failed:" << e.what();
    }
}

void RpcClient::sendMessage(const QString &method, const QString &message) {
    if (!client) {
        emit errorOccurred("Not connected to server");
        return;
    }

    Request request;
    request.method = method.toStdString(); // 动态指定方法名
    request.params = message.toStdString();

    try {
        Response response = client->call(method.toStdString(), request).as<Response>();
        emit messageReceived(QString::fromStdString(response.result));
        qDebug() << "Method" << method << "called successfully. Response:" << QString::fromStdString(response.result);
    } catch (const std::exception &e) {
        emit errorOccurred(QString::fromStdString(e.what()));
        qWarning() << "Failed to call method" << method << ":" << QString::fromStdString(e.what());
    }
}