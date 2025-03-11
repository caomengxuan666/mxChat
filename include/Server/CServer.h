#pragma once

#include "const.h"
#include <Server/CSession.h>

/**
 * @author       : caomengxuan666
 * @brief        : 聊天服务器，管理会话Session
**/
class CServer : public std::enable_shared_from_this<CServer> {
public:
    CServer(boost::asio::io_context &ioc, unsigned short &port);

    /**
     * @author       : caomengxuan666
     * @brief        : 根据uuid清除Session
     * @param         {string} uuid:
     * @return        {*}
    **/    
    void ClearSession(std::string uuid);

private:
    /**
     * @author       : caomengxuan666
     * @brief        : 管理会话的连接
     * @param         {error_code} &error:
     * @return        {*}
    **/
    void HandleAccept(std::shared_ptr<CSession>, const boost::system::error_code &error);

    /**
     * @author       : caomengxuan666
     * @brief        : 进行和会话的连接，创建Socket
     * @return        {*}
    **/    
    void StartAccept();

private:
    tcp::acceptor _acceptor;

    boost::asio::io_context &_io_context;
    short _port;
    std::map<std::string, std ::shared_ptr<CSession>> _sessions;
    std::mutex _mutex;
};