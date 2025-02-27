// StatusMonitor.h
/**
 * @FilePath     : /mxChat/include/Server/StatusMonitor.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-26 20:44:34
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include "StatusServiceImpl.h"
#include <asio.hpp>
#include <string>

class StatusMonitor {
public:
    using Address = std::pair<std::string, size_t>;

    static StatusMonitor *GetInstance(StatusServiceImpl *statusService) {
        if (_instance == nullptr) {
            _instance = new StatusMonitor(statusService);
        }
        return _instance;
    }

    StatusMonitor(const StatusMonitor &) = delete;
    StatusMonitor &operator=(const StatusMonitor &) = delete;

    void UpdateServerStatus();// 观察者接口
    void PrintServerStatus(); // 遍历输出接口

    // 启动外部接口服务
    void StartExternalInterface(uint16_t port);

private:
    StatusServiceImpl *_statusService;
    ServerStatus _serverInfo;

    StatusMonitor(StatusServiceImpl *statusService) : _statusService(statusService) {}

    static StatusMonitor *_instance;

    // 处理客户端请求的函数
    void HandleClient(asio::ip::tcp::socket socket);

    // 将服务器状态转换为字符串格式
    std::string GetServerStatusString();

    std::thread _serverThread;// 用于运行 TCP 服务器的线程
    bool _isRunning = false;  // 标记服务器是否正在运行
    std::mutex _mutex;        // 保护共享资源
};