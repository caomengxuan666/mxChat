// StatusMonitor.h
/**
 * @FilePath     : /mxChat/include/Server/StatusMonitor.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-24 21:39:02
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include "StatusServiceImpl.h"
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

    // 遍历输出接口
    void PrintServerStatus();

    // 我觉得以后这里可以加一个remote的接口，从远程获取服务器运行状态。

private:
    StatusServiceImpl *_statusService;
    ServerInfo _serverInfo;
    StatusMonitor(StatusServiceImpl *statusService) : _statusService(statusService) {}

    static StatusMonitor *_instance;
};
