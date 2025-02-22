#include "DataBase/MysqlMgr.h"
#include "DataBase/RedisMgr.h"
#include "Server/AsioIOServicePool.h"
#include "Server/StatusServiceImpl.h"
#include <QCoreApplication>
#include <QDebug>
#include <Server/CServer.h>
#include <Server/config.hpp>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>

#ifdef INTERNAL
#undef INTERNAL
#endif

#undef signals

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    try {
        // 获取 Config_Manager 单例实例
        auto &config_manager = Config_Manager::getInstance();

        // 设置并加载配置文件
        config_manager.setYamlPath("server.yaml");
        YAML::Node config = config_manager.loadYamlDoc();

        // 启动GRPC
        auto host = config_manager["StatusServer"]["host"].as<std::string>();
        auto port = config_manager["StatusServer"]["port"].as<std::string>();
        StatusServiceImpl service;
        grpc::ServerBuilder builder;

        std::string server_address = host + ":" + port;
        // 监听端口和添加服务
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);

        // 构建并启动gRPC服务器
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        spdlog::info("Status Server listening on :{}", server_address);

        // 从配置文件中读取 CServer 的配置
        std::string cServerHost = config["CServer"]["host"].as<std::string>();
        unsigned short cServerPort = config["CServer"]["port"].as<unsigned short>();

        // 启动新的 CServer 服务器
        spdlog::info("Starting CServer on {}:{}", cServerHost, cServerPort);
        boost::asio::io_context io_context{1};
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);

        // 设置异步等待SIGINT信号
        signals.async_wait([&server, &io_context](const boost::system::error_code &error, int signal_number) {
            if (!error) {
                std::cout << "Shutting down server..." << std::endl;
                server->Shutdown(); // 优雅地关闭gRPC服务器
                io_context.stop();  // 停止io_context
            }
        });

        // 启动CServer
        auto cserver = std::make_shared<CServer>(io_context, cServerPort);
        cserver->Start();

        // 运行io_context
        io_context.run();

        // 等待服务器关闭
        server->Wait();
    } catch (const YAML::Exception &yamlEx) {
        spdlog::critical("YAML parsing error: {}", yamlEx.what());
        return EXIT_FAILURE;
    } catch (const std::exception &ex) {
        spdlog::critical("Failed to initialize servers: {}", ex.what());
        return EXIT_FAILURE;
    }

    return a.exec();
}