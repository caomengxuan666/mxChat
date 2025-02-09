/**
 * @FilePath     : /mxChat/include/Server/RpcServer.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-09 00:26:18
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#ifndef RPCSERVER_H
#define RPCSERVER_H

#include "Server.h"
#include <rpc/server.h>
#include <QString>

class RpcServer : public Server {
    Q_OBJECT

public:
    explicit RpcServer(QObject *parent = nullptr);
    ~RpcServer() override;

    void startServer(quint16 port) override;

protected:
    void onReadyRead() override {}
    void onClientDisconnected() override {}

private:
    rpc::server *server;
};

#endif // RPCSERVER_H