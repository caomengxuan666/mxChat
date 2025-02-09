#ifndef RPCCLIENT_H
#define RPCCLIENT_H

#include "Client.h"
#include <rpc/client.h>

class RpcClient : public Client {
    Q_OBJECT

public:
    explicit RpcClient(QObject *parent = nullptr);
    ~RpcClient() override;

    void connectToServer(const QString &address, quint16 port) override;
    void sendMessage(const QString &method, const QString &message) override;

protected:
    void onReadyRead() override {}
    void onConnected() override {}
    void onDisconnected() override {}
    void onError() override {}

private:
    rpc::client *client;
};

#endif // RPCCLIENT_H