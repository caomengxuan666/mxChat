#include <iostream>
#include <Server/CServer.h>
#include <Server/config.hpp>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>

#ifdef INTERNAL
#undef INTERNAL
#endif

int main(int argc, char *argv[]) {
    try {
        // 获取 Config_Manager 单例实例
        auto &config_manager = Config_Manager::getInstance();

        // 设置并加载配置文件
        config_manager.setYamlPath("server.yaml");
        YAML::Node config = config_manager.loadYamlDoc();

        // 从配置文件中读取 CServer 的配置
        std::string cServerHost = config["CServer"]["host"].as<std::string>();
        unsigned short cServerPort = config["CServer"]["port"].as<unsigned short>();

        // 启动CServer 服务器
        spdlog::info("Starting CServer on {}:{}", cServerHost, cServerPort);
        boost::asio::io_context io_context{1};
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);

        // 设置异步等待SIGINT信号b
        signals.async_wait([&io_context](const boost::system::error_code &error, int signal_number) {
            if (!error) {
                std::cout << "Shutting down server..." << std::endl;
                io_context.stop();  // 停止io_context
            }
        });

        // 启动CServers
        auto cserver = std::make_shared<CServer>(io_context, cServerPort);
        cserver->Start();

        // 运行io_context
        io_context.run();

    } catch (const YAML::Exception &yamlEx) {
        spdlog::critical("YAML parsing error: {}", yamlEx.what());
        return EXIT_FAILURE;
    } catch (const std::exception &ex) {
        spdlog::critical("Failed to initialize servers: {}", ex.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}