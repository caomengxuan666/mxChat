// main_server.cpp
#include <QCoreApplication>
#include <QDebug>
#include <Server/HttpServer.h>
#include <Server/RpcServer.h>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    RpcServer rpcserver;
    constexpr quint16 port = 8080;

    HttpServer httpserver;
    rpcserver.startServer(port);

    httpserver.startServer(port + 1);

    return a.exec();
}
