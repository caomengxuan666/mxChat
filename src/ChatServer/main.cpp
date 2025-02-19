#include <QCoreApplication>
#include <QDebug>
#include <Server/HttpServer.h>
#include <Server/RpcServer.h>
#include <Server/CServer.h>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include <Server/config.hpp>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    try {
        // 获取 Config_Manager 单例实例
        auto& config_manager = Config_Manager::getInstance();

        // 设置并加载配置文件
        config_manager.setYamlPath("server.yaml");
        YAML::Node config = config_manager.loadYamlDoc();

        // 从配置文件中读取 RPC 服务器的配置
        std::string rpcHost = config["RpcServer"]["host"].as<std::string>();
        quint16 rpcPort = config["RpcServer"]["port"].as<quint16>();

        // 启动 RPC 服务器
        RpcServer rpcserver;
        spdlog::info("Starting RPC server on {}:{}", rpcHost, rpcPort);
        rpcserver.startServer(rpcPort);

        // 从配置文件中读取 CServer 的配置
        std::string cServerHost = config["CServer"]["host"].as<std::string>();
        unsigned short cServerPort = config["CServer"]["port"].as<unsigned short>();

        // 启动新的 CServer 服务器
        spdlog::info("Starting CServer on {}:{}", cServerHost, cServerPort);
        boost::asio::io_context ioc{1};
        boost::asio::signal_set signalSet(ioc, SIGINT, SIGTERM); // 将 signals 改为 signalSet
        signalSet.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
            if (error) {
                return;
            }
            ioc.stop();
        });
        std::make_shared<CServer>(ioc, cServerPort)->Start();
        ioc.run();
    } catch (const YAML::Exception& yamlEx) {
        spdlog::critical("YAML parsing error: {}", yamlEx.what());
        return EXIT_FAILURE;
    } catch (const std::exception& ex) {
        spdlog::critical("Failed to initialize servers: {}", ex.what());
        return EXIT_FAILURE;
    }

    return a.exec();
}