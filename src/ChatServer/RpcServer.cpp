#include "Server/RpcServer.h"
#include "msg.hpp"
#include <QDebug>
#include<DataBase/DataBaseClient.h>

RpcServer::RpcServer(QObject *parent)
    : Server(parent), server(nullptr) {}

RpcServer::~RpcServer() {
    delete server;
}

void RpcServer::startServer(quint16 port) {
    try {
        server = new rpc::server(port);

        // 绑定 "process" 方法
        server->bind("process", [](const Request &request) -> Response {
            Response response;
            response.status = 200;
            response.result = "Processed: " + request.params;
            return response;
        });

        // 绑定 "send" 方法
        server->bind("send", [](const Request &request) -> Response {
            Response response;
            response.status = 200;
            response.result = "Message sent: " + request.params;
            qDebug() << "Server received send request with params:" << QString::fromStdString(request.params);
            return response;
        });

        // 绑定 "login" 方法
        server->bind("login", [](const Request &request) -> Response {
            Response response;
            // 假设请求参数格式为 "username,password"
            QStringList parts = QString::fromStdString(request.params).split(",");
            if (parts.size() != 2) {
                response.status = 400;
                response.result = "Bad request";
                return response;
            }

            QString username = parts[0];
            QString password = parts[1];

            // 连接到数据库验证用户
            DataBaseClient db;
            if (!db.connectToDatabase("/home/cmx/QtProjects/mxChat/data/users.db")) {
                response.status = 500;
                response.result = "Internal server error";
                return response;
            }

            QString query = "SELECT * FROM Users WHERE Username = ? AND PasswordHash = ?";
            QVariantList params;
            params << username << password;

            bool userExists = db.executeQueryWithParams(query, params);

            if (userExists) {
                response.status = 200;
                response.result = "Login successful";
                qDebug() << "Login successful for user:" << username;
            } else {
                response.status = 401;
                response.result = "Login failed: Invalid credentials";
                qDebug() << "Login failed for user:" << username;
            }

            return response;
        });

        // 启动服务器，使用 4 个线程运行
        server->async_run(4);
        qDebug() << "RPC Server is running on port" << port;

    } catch (const std::exception &e) {
        qWarning() << "Failed to start server:" << QString::fromStdString(e.what());
    }
}