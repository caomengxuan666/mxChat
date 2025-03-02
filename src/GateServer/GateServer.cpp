/**
 * @FilePath     : /mxChat/src/GateServer/GateServer.cpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-02 22:50:40
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include <Server/AsioIOServicePool.h>
#include <Server/GateServer.h>
#include <Server/HttpConnection.h>

GateServer::GateServer(boost::asio::io_context& ioc, unsigned short& port) :_ioc(ioc),
_acceptor(ioc, tcp::endpoint(tcp::v4(), port)),_socket(ioc) {

}

void GateServer::Start() {
    auto self = shared_from_this();
    auto &io_context = AsioIOServicePool::GetInstance()->GetIOService();
    tcp::socket new_socket(io_context);                                                               // 创建一个新的 tcp::socket 对象
    std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(std::move(new_socket));// 传递 socket 对象
    _acceptor.async_accept(new_con->GetSocket(), [self, new_con](beast::error_code ec) {
        try {
            //出错则放弃这个连接，继续监听新链接
            if (ec) {
             self->Start();
                return;
            }

            //处理新链接，创建HttpConnection类管理新连接
            new_con->Start();
            //继续监听
            self->Start();
        } catch (std::exception &exp) {
            std::cout << "exception is " << exp.what() << std::endl;
            self->Start();
        }
    });
}   