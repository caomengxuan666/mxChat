/**
 * @FilePath     : /mxChat/include/Server/StatusMonitor.h
 * @Description  :  服务器状态监控器
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 18:35:36
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include "StatusServiceImpl.h"
#include <asio.hpp>
#include <string>

/**
 * @author       : caomengxuan666
 * @brief        : 服务器的监视者类,作为 StatusServiceImpl 的观察者,开放外部端口以使得可以随时获取服务器负载状态
**/

class StatusMonitor {
public:
    using Address = std::pair<std::string, size_t>;

    /**
     * @author       : caomengxuan666
     * @brief        : 获取单例监视者对象
     * @param         {StatusServiceImpl} *statusService:
     * @return        {StatusMonitor} 返回单例监视者对象
    **/    
    static StatusMonitor *GetInstance(StatusServiceImpl *statusService) {
        if (_instance == nullptr) {
            _instance = new StatusMonitor(statusService);
        }
        return _instance;
    }

    StatusMonitor(const StatusMonitor &) = delete;
    StatusMonitor &operator=(const StatusMonitor &) = delete;

    /**
     * @author       : caomengxuan666
     * @brief        : 更新服务器状态
     * @return        {*}
    **/    
    void UpdateServerStatus();// 观察者接口
    /**
     * @author       : caomengxuan666
     * @brief        : 打印服务器状态
     * @return        {*}
    **/    
    void PrintServerStatus(); // 遍历输出接口

    /**
     * @author       : caomengxuan666
     * @brief        : 启动外部接口,提供状态查询服务
     * @param         {uint16_t} port:
     * @note         : 可以通过echo "STATUS" | nc localhost 8080获取服务状态。
     * @return        {*}
    **/    
    void StartExternalInterface(uint16_t port);

private:
    StatusServiceImpl *_statusService;
    ServerStatus _serverInfo;

    StatusMonitor(StatusServiceImpl *statusService) : _statusService(statusService) {}

    static StatusMonitor *_instance;
    
    /**
     * @author       : caomengxuan666
     * @brief        : 处理客户端请求
     * @param         {socket} socket:
     * @return        {*}
    **/    
    void HandleClient(asio::ip::tcp::socket socket);

    /**
     * @author       : caomengxuan666
     * @brief        : 获取服务器状态字符串
     * @return        {*}
    **/    
    std::string GetServerStatusString();

    std::thread _serverThread;// 用于运行 TCP 服务器的线程
    bool _isRunning = false;  // 标记服务器是否正在运行
    std::mutex _mutex;        // 保护共享资源
};