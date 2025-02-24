/**
 * @FilePath     : /mxChat/src/ChatServer/StatusServer.cpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-24 21:37:16
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/

#include "Server/AsioIOServicePool.h"
#include "Server/StatusServiceImpl.h"
#include "Server/const.h"
#include <Server/StatusMonitor.h>
#include <Server/config.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include <string>
#include <thread>

#ifdef INTERNAL
#undef INTERNAL
#endif

// 去除字符串两端的空格
inline static std::string trim(const std::string &str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) start++;
    auto end = str.end();
    do { end--; } while (std::distance(start, end) > 0 && std::isspace(*end));
    return std::string(start, end + 1);
}

void RunStatusServer() {
    auto &cfg = Config_Manager::getInstance();
    cfg.setYamlPath("server.yaml");

    //std::string host = trim(cfg["StatusServer"]["host"].as<std::string>());
    std::string port = trim(cfg["StatusServer"]["port"].as<std::string>());

    std::string server_address = "0.0.0.0:" + port;

    StatusServiceImpl service;
    // 绑定观察者
    StatusMonitor *monitor = StatusMonitor::GetInstance(&service);

    grpc::ServerBuilder builder;

    // 监听 IPv4 地址
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    // 构建并启动 gRPC 服务器
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // 创建 Boost.Asio 的 io_context
    boost::asio::io_context io_context;

    // 创建 signal_set 捕获 SIGINT
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&server](const boost::system::error_code &error, int signal_number) {
        if (!error) {
            std::cout << "Shutting down server..." << std::endl;
            server->Shutdown();// 关闭服务器
        }
    });

    // 在单独的线程中运行 io_context
    std::thread io_thread([&io_context]() {
        spdlog::info("io_context thread started");
        io_context.run();
        spdlog::info("io_context thread exited");
    });
    io_thread.detach();

    // 等待服务器关闭
    server->Wait();
    io_context.stop();// 停止 io_context
}


int main(int argc, char **argv) {
    try {
        RunStatusServer();
    } catch (std::exception const &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}