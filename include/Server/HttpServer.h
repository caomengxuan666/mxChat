/**
 * @FilePath     : /mxChat/include/Server/HttpServer.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-09 13:22:11
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include "Server.h"
#include <QTcpServer>

class HttpServer : public Server {
    Q_OBJECT

public:
    explicit HttpServer(QObject *parent = nullptr);
    ~HttpServer() override;

    void startServer(quint16 port) override;

protected:
    void onReadyRead() override;
    void onClientDisconnected() override;
    void onNewConnection() ;
private:
    QTcpServer *server;
};