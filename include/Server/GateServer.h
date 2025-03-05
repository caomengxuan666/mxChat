/**
 * @FilePath     : /mxChat/include/Server/GateServer.h
 * @Description  :  GateServer服务器
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 17:59:11
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once
#include "const.h"

/**
 * @author       : caomengxuan
 * @brief        : GateServer服务器,作为网关接受服务器的连接,处理来自客户端的注册,登录，重置密码等等Http请求
**/
class GateServer : public std::enable_shared_from_this<GateServer> {
public:
    /**
     * @author       : caomengxuan
     * @brief        : 初始化网关服务器
     * @param         {io_context} &ioc:
     * @param         {unsigned short} &port:
     * @return        {*}
    **/
    GateServer(boost::asio::io_context &ioc, unsigned short &port);
    /**
     * @author       : caomengxuan
     * @brief        : 启动网关服务器
     * @return        {*}
    **/    
    void Start();

private:
    tcp::acceptor _acceptor;
    net::io_context &_ioc;
    boost::asio::ip::tcp::socket _socket;
};
