// main_server.cpp
#include <QCoreApplication>
#include <QDebug>
#include <Server/HttpServer.h>
#include <Server/RpcServer.h>
#include <Server/CServer.h>
#include <boost/asio.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // 启动 RPC 服务器
    RpcServer rpcserver;
    constexpr quint16 rpcPort = 8080;
    rpcserver.startServer(rpcPort);

    // 启动 HTTP 服务器
    HttpServer httpserver;
    httpserver.startServer(rpcPort + 1);

    // 启动新的 CServer 服务器
    try {
        unsigned short cServerPort = static_cast<unsigned short>(8082); // 使用不同的端口
        spdlog::info("Starting CServer on port {}", cServerPort);
        boost::asio::io_context ioc{ 1 };
        boost::asio::signal_set signalSet(ioc, SIGINT, SIGTERM); // 将 signals 改为 signalSet
        signalSet.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
            if (error) {
                return;
            }
            ioc.stop();
        });
        std::make_shared<CServer>(ioc, cServerPort)->Start();
        ioc.run();
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return a.exec();
}