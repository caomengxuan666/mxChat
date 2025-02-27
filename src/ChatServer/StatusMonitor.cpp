#include "Server/StatusMonitor.h"
#include "Server/StatusServiceImpl.h"
#include <spdlog/spdlog.h>

// 初始化静态成员变量
StatusMonitor *StatusMonitor::_instance = nullptr;
void StatusMonitor::UpdateServerStatus() {
    // 更新服务器状态的逻辑
    if (_statusService) {
        // 访问 _statusService 的状态
        this->_serverInfo = _statusService->serverInfo();
        PrintServerStatus();
    }
}

void StatusMonitor::PrintServerStatus() {
    const ServerStatus &serverInfo = _statusService->serverInfo();

    // 打印总连接数
    spdlog::info("当前服务器连接总数: {}", serverInfo.totalConnection);

    // 打印绑定地址及其连接数
    spdlog::info("Bind Address Status:");
    for (const auto& [serverId, addressInfo] : serverInfo.bindAddressStatus) {
        std::string host = addressInfo.first;
        size_t connectionCount = addressInfo.second;
        spdlog::info("服务器ID: {}, 地址: {}, 连接数: {}", serverId, host, connectionCount);
    }
}

std::string StatusMonitor::GetServerStatusString() {
    std::lock_guard<std::mutex> lock(_mutex);
    std::ostringstream oss;
    oss << "当前服务器连接总数: " << _serverInfo.totalConnection << "\n";
    oss << "Bind Address Status:\n";
    for (const auto &[id, address]: _serverInfo.bindAddressStatus) {
        oss << "ID: " << id << ", Address: " << address.first << ":" << address.second
            << ", 连接数: " << address.second << "\n";
    }
    return oss.str();
}
void StatusMonitor::HandleClient(asio::ip::tcp::socket socket) {
    try {
        asio::streambuf buffer;
        asio::read_until(socket, buffer, "\n");// 读取客户端指令
        std::istream is(&buffer);
        std::string command;
        std::getline(is, command);

        if (command == "STATUS") {
            std::string status = GetServerStatusString();
            asio::write(socket, asio::buffer(status));// 返回服务器状态
        } else {
            asio::write(socket, asio::buffer("未知指令\n"));
        }
    } catch (std::exception &e) {
        spdlog::error("处理客户端请求时发生错误: {}", e.what());
    }
}

void StatusMonitor::StartExternalInterface(uint16_t port) {
    _isRunning = true;
    _serverThread = std::thread([this, port]() {
        try {
            asio::io_context io_context;
            asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
            spdlog::info("外部接口服务已启动，监听端口: {}", port);

            while (_isRunning) {
                asio::ip::tcp::socket socket(io_context);
                acceptor.accept(socket);
                HandleClient(std::move(socket));
            }
        } catch (std::exception &e) {
            spdlog::error("外部接口服务启动失败: {}", e.what());
        }
    });
}