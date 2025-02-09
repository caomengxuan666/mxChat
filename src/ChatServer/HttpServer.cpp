#include "Server/HttpServer.h"
#include <QTcpSocket>
#include <QDebug>

HttpServer::HttpServer(QObject *parent)
    : Server(parent), server(new QTcpServer(this)) {
    connect(server, &QTcpServer::newConnection, this, &HttpServer::onNewConnection);
}

HttpServer::~HttpServer() {
    delete server;
}

void HttpServer::startServer(quint16 port) {
    if (!server->listen(QHostAddress::Any, port)) {
        qWarning() << "Failed to start HTTP server:" << server->errorString();
        return;
    }
    qDebug() << "HTTP Server is running on port" << port;
}

void HttpServer::onNewConnection() {
    QTcpSocket *clientSocket = server->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &HttpServer::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &HttpServer::onClientDisconnected);
}

void HttpServer::onReadyRead() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket) {
        qWarning() << "Invalid client socket";
        return;
    }

    QByteArray data = clientSocket->readAll();
    qDebug() << "Received data:" << data;

    // 处理 HTTP 请求
    QString request = QString::fromUtf8(data);
    // 这里可以添加解析 HTTP 请求的逻辑
    // 例如，解析请求方法、路径、参数等

    // 示例响应
    QString response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nHello, World!";
    clientSocket->write(response.toUtf8());
    clientSocket->disconnectFromHost();
}

void HttpServer::onClientDisconnected() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket) {
        qWarning() << "Invalid client socket";
        return;
    }

    qDebug() << "Client disconnected:" << clientSocket->peerAddress().toString();
    clientSocket->deleteLater();
}