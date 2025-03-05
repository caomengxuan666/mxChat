/**
 * @FilePath     : /mxChat/include/Server/AsioIOServicePool.h
 * @Description  :  ASIOd io_service 线程池
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 17:29:28
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once
#include "Singleton.h"
#include <boost/asio.hpp>
#include <vector>

/**
 * @author       : caomengxuan
 * @brief        : ASIO io_service 线程池
**/
class AsioIOServicePool : public Singleton<AsioIOServicePool> {
    friend Singleton<AsioIOServicePool>;

public:
    using IOService = boost::asio::io_context;
    using Work = boost::asio::io_context::work;
    using WorkPtr = std::unique_ptr<Work>;
    ~AsioIOServicePool();
    AsioIOServicePool(const AsioIOServicePool &) = delete;
    AsioIOServicePool &operator=(const AsioIOServicePool &) = delete;

    /**
     * @author       : caomengxuan
     * @brief        : 使用 round-robin 的方式返回一个 io_service
     * @return        {*}
    **/    
    boost::asio::io_context &GetIOService();
    /**
     * @author       : caomengxuan
     * @brief        : 使得 io_service从run的状态退出
     * @return        {*}
    **/    
    void Stop();

private:
    /**
     * @author       : caomengxuan
     * @brief        : 构造函数，初始化线程池，每个线程内部启动一个io_service
     * @param         {size_t} size:
     * @return        {*}
    **/
    AsioIOServicePool(std::size_t size = 2 /*std::thread::hardware_concurrency()*/);
    std::vector<IOService> _ioServices;
    std::vector<WorkPtr> _works;
    std::vector<std::thread> _threads;
    std::size_t _nextIOService;
};