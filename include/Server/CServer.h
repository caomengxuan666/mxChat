#pragma once

#include "const.h"
#include <Server/CSession.h>
class CServer : public std::enable_shared_from_this<CServer> {
public:
    CServer(boost::asio::io_context &ioc, unsigned short &port);
    void ClearSession(std::string);

private:
    void HandleAccept(std::shared_ptr<CSession>, const boost::system::error_code &error);
    void StartAccept();

private:
    tcp::acceptor _acceptor;

    boost::asio::io_context &_io_context;
    short _port;
    std::map<std::string, std ::shared_ptr<CSession>> _sessions;
    std::mutex _mutex;
};