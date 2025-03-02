/**
 * @FilePath     : /mxChat/include/Server/StatusServiceImpl.h
 * @Description  :  echo "STATUS" | nc localhost 8080获取服务状态。
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-02 11:21:08
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once
#include "message.pb.h"
#ifdef INTERNAL
#undef INTERNAL
#endif
#include "message.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <vector>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;
using message::LoginRsp;
using message::LoginReq;

class StatusMonitor;

struct ChatServer {
    std::string host;
    std::string port;
};

struct ServerStatus {
    // 对应 ip:port
    using Address = std::pair<std::string, size_t>;
    // 绑定的 ip:port 及其接受的连接数
    std::unordered_map<size_t, std::pair<std::string, size_t>> bindAddressStatus;
    // 总共连接到服务器的数量
    size_t totalConnection = 0;
};

class StatusServiceImpl final : public StatusService::Service {
public:
    StatusServiceImpl();
    Status GetChatServer(ServerContext *context, const GetChatServerReq *request,
                         GetChatServerRsp *reply) override;
    Status Login(ServerContext *context, const LoginReq *request, LoginRsp *reply)override;
    void insertToken(int uid, std::string token);

    void addObserver(StatusMonitor *observer);
    void removeObserver(StatusMonitor *observer);
    void notifyObservers();
    const ServerStatus &serverInfo() const;

protected:
    std::vector<ChatServer> _servers;
    int _server_index;
    std::vector<StatusMonitor *> _observers;

private:
    void addOberver();
    StatusMonitor *_monitor;
    ServerStatus _serverInfo;
};