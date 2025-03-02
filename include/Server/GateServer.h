/**
 * @FilePath     : /mxChat/include/Server/GateServer.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-02 22:51:14
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once
#include "const.h"

class GateServer : public std::enable_shared_from_this<GateServer> {
public:
    GateServer(boost::asio::io_context &ioc, unsigned short &port);
    void Start();

private:
    tcp::acceptor _acceptor;
    net::io_context &_ioc;
    boost::asio::ip::tcp::socket _socket;
};
