#include "Server/CServer.h"
#include "Server/AsioIOServicePool.h"
#include "Server/HttpConnection.h"
#include <spdlog/spdlog.h>
#include<Server/UserMgr.h>

CServer::CServer(boost::asio::io_context &ioc, unsigned short &port)
    : _ioc(ioc),
      _acceptor(ioc, tcp::endpoint(tcp::v4(), port)),
      _socket(ioc),
      _io_context(ioc),// 添加对 _io_context 的初始化
      _port(port) {
    StartAccept();
}

void CServer::Start() {
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

void CServer::StartAccept() {
    auto &io_context = AsioIOServicePool::GetInstance()->GetIOService();
    shared_ptr<CSession> new_session = make_shared<CSession>(io_context, this);
    _acceptor.async_accept(new_session->GetSocket(), std::bind(&CServer::HandleAccept, this, new_session, placeholders::_1));
}

void CServer::ClearSession(std::string uuid) {
	
	if (_sessions.find(uuid) != _sessions.end()) {
		//移除用户和session的关联
		UserMgr::GetInstance()->RmvUserSession(_sessions[uuid]->GetUserId());
	}

	{
		lock_guard<mutex> lock(_mutex);
		_sessions.erase(uuid);
	}
	
}
 
void CServer::HandleAccept(shared_ptr<CSession> new_session, const boost::system::error_code& error){
	if (!error) {
		new_session->Start();
		lock_guard<mutex> lock(_mutex);
		_sessions.insert(make_pair(new_session->GetUuid(), new_session));
	}
	else {
		cout << "session accept failed, error is " << error.what() << endl;
	}

	StartAccept();
}