#pragma once
#ifdef INTERNAL
#undef INTERNAL
#endif
#include "message.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <vector>
#include <functional>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;

class StatusMonitor;

struct ChatServer {
    std::string host;
    std::string port;
};

struct ServerStatus {
    //对应ip:port
    using Address = std::pair<std::string, size_t>;
    //绑定的ip:port各自接受的连接数
    std::unordered_map<size_t, Address> bindAddressStatus;
    //总共连接到服务器的数量
    size_t totalConnection;
};

class StatusServiceImpl final : public StatusService::Service {
public:
    StatusServiceImpl();
    Status GetChatServer(ServerContext *context, const GetChatServerReq *request,
                         GetChatServerRsp *reply) override;

    void addObserver(StatusMonitor* observer);
    void removeObserver(StatusMonitor* observer);
    void notifyObservers();
    const ServerStatus& serverInfo()const;

protected:
    std::vector<ChatServer> _servers;
    int _server_index;
    std::vector<StatusMonitor*> _observers;

private:
    void addOberver();
    StatusMonitor *_monitor;
    ServerStatus _serverInfo;
};