#include "Server/CServer.h"
#include "Server/AsioIOServicePool.h"
#include "Server/HttpConnection.h"
#include <Server/UserMgr.h>
#include <spdlog/spdlog.h>

CServer::CServer(boost::asio::io_context &ioc, unsigned short &port)
    : _acceptor(ioc, tcp::endpoint(tcp::v4(), port)),
      _io_context(ioc),// 添加对 _io_context 的初始化
      _port(port) {
    StartAccept();
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

void CServer::HandleAccept(shared_ptr<CSession> new_session, const boost::system::error_code &error) {
    if (!error) {
        new_session->Start();
        lock_guard<mutex> lock(_mutex);
        _sessions.insert(make_pair(new_session->GetUuid(), new_session));
        cout << "session accept success at " << new_session->GetUuid() << " "<< endl;
    } else {
        cout << "session accept failed, error is " << error.what() << endl;
    }

    StartAccept();
}