#include "Server/StatusMonitor.h"
#include "Server/StatusServiceImpl.h"
#include <spdlog/spdlog.h>

// 初始化静态成员变量
StatusMonitor* StatusMonitor::_instance = nullptr;
void StatusMonitor::UpdateServerStatus() {
    // 更新服务器状态的逻辑
    if (_statusService) {
        // 访问 _statusService 的状态
        this->_serverInfo = _statusService->serverInfo();
        PrintServerStatus();
    }
}

void StatusMonitor::PrintServerStatus() {
    // 获取 ServerInfo 对象
    const ServerInfo &serverInfo = _statusService->serverInfo();

    // 打印总连接数
    spdlog::info("Total Connections: {}", serverInfo.totalConnection);

    // 打印绑定地址及其连接数
    spdlog::info("Bind Address Status:");
    for (const auto &[id, address]: serverInfo.bindAddressStatus) {
        spdlog::info("服务器ID: {}, Address: {}:{}", id, address.first, address.second);
    }
}