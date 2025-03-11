/**
 * @FilePath     : /mxChat/include/Server/StatusServiceImpl.h
 * @Description  : 状态服务器的实现类,同时作为被观察者
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 21:10:55
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
using message::LoginReq;
using message::LoginRsp;
using message::StatusService;

class StatusMonitor;

/**
 * @author       : caomengxuan666
 * @brief        : 服务器的ip和端口
**/
struct ChatServer {
    std::string host;
    std::string port;
};

/**
 * @author       : caomengxuan666
 * @brief        : 服务器的状态结构体
**/
struct ServerStatus {
    // 对应 ip:port
    using Address = std::pair<std::string, size_t>;
    // 绑定的 ip:port 及其接受的连接数
    std::unordered_map<size_t, std::pair<std::string, size_t>> bindAddressStatus;
    // 总共连接到服务器的数量
    size_t totalConnection = 0;
};

/**
 * @author       : caomengxuan666
 * @brief        : 状态服务器的实现类
**/
class StatusServiceImpl final : public StatusService::Service {
public:
    StatusServiceImpl();
    /**
     * @author       : caomengxuan666
     * @brief        : 获取服务器的状态信息,设置rpc的reply,返回选中的负载均衡服务器的host和port,并且将Token写入redis
     * @return        {Status}:服务器的状态
    **/
    Status GetChatServer(ServerContext *context, const GetChatServerReq *request,
                         GetChatServerRsp *reply) override;
    /**
     * @author       : caomengxuan666
     * @brief        : 登录请求,设置rpc的reply,设置reply,返回登录成功与否
     * @param         {ServerContext} *context:
     * @param         {LoginReq} *request:
     * @param         {LoginRsp} *reply:
     * @return        {Status}:服务器的状态
    **/
    Status Login(ServerContext *context, const LoginReq *request, LoginRsp *reply) override;

    /**
     * @author       : caomengxuan666
     * @brief        : 执行操作,拼接uid和token,将其写入redis
     * @param         {int} uid:
     * @param         {string} token:
     * @return        {*}
    **/
    void insertToken(int uid, std::string token);

    /**
     * @author       : caomengxuan666
     * @brief        : 绑定观察者对象,用于添加StatusMonitor
     * @param         {StatusMonitor} *observer:
     * @return        {*}
    **/

    void addObserver(StatusMonitor *observer);
    /**
     * @author       : caomengxuan666
     * @brief        : 移除观察者对象,用于移除StatusMonitor
     * @param         {StatusMonitor} *observer:
     * @return        {*}
    **/    
    void removeObserver(StatusMonitor *observer);
    /**
     * @author       : caomengxuan666
     * @brief        : 通知观察者,更新服务器状态
     * @return        {*}
    **/
    void notifyObservers();
    /**
     * @author       : caomengxuan666
     * @brief        : 获取服务器状态
     * @return        {const ServerStatus &}服务器状态结构体
    **/    
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