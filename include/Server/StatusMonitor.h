// StatusMonitor.h
/**
 * @FilePath     : /mxChat/include/Server/StatusMonitor.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-24 21:33:07
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include "StatusServiceImpl.h"
#include <string>

class StatusMonitor {
public:
    using Address = std::pair<std::string, size_t>;

    // 获取单例实例
    static StatusMonitor* GetInstance(StatusServiceImpl *statusService) {
        if (_instance == nullptr) {
            _instance = new StatusMonitor(statusService);
        }
        return _instance;
    }

    // 删除拷贝构造函数和赋值操作符
    StatusMonitor(const StatusMonitor&) = delete;
    StatusMonitor& operator=(const StatusMonitor&) = delete;

    void UpdateServerStatus(); // 观察者接口

    // 遍历输出接口
    void PrintServerStatus();

private:
    StatusServiceImpl *_statusService;
    ServerInfo _serverInfo;

    // 私有构造函数
    StatusMonitor(StatusServiceImpl *statusService) : _statusService(statusService) {}

    // 静态成员变量，存储唯一的实例
    static StatusMonitor* _instance;
};

