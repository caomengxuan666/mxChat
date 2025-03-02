/**
 * @FilePath     : /mxChat/src/GateServer/Gate.cpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-02 22:53:19
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include <Server/AsioIOServicePool.h>
#include <Server/GateServer.h>
#include <Server/config.hpp>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>

#ifdef INTERNAL
#undef INTERNAL
#endif

bool bstop = false;
std::condition_variable cond_quit;
std::mutex mutex_quit;

int main(int argc, char *argv[]) {
    try {
        // 获取 Config_Manager 单例实例
        auto &config_manager = Config_Manager::getInstance();

        // 设置并加载配置文件
        config_manager.setYamlPath("server.yaml");
        YAML::Node config = config_manager.loadYamlDoc();

        // 从配置文件中读取 GateServer 的配置
        std::string gateServerHost = config["GateServer"]["host"].as<std::string>();
        unsigned short gateServerPort = config["GateServer"]["port"].as<unsigned short>();

        auto pool = AsioIOServicePool::GetInstance();

        // 启动GateServer 服务器
        spdlog::info("Starting GateServer on {}:{}", gateServerHost, gateServerPort);
        boost::asio::io_context io_context{1};
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);

        // 设置异步等待SIGINT信号b
        signals.async_wait([&io_context, pool](const boost::system::error_code &error, int signal_number) {
            if (!error) {
                std::cout << "Shutting down server..." << std::endl;
                io_context.stop();// 停止io_context
                pool->Stop();
            }
        });

        auto gateserver = std::make_shared<GateServer>(io_context, gateServerPort);
        gateserver->Start();
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