/**
 * @FilePath     : /mxChat/src/ChatServer/StatusServiceImpl.cpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-26 21:19:47
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include "Server/StatusMonitor.h"
#include <Server/StatusServiceImpl.h>
#include <Server/config.hpp>
#include <Server/const.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <grpcpp/support/status.h>
#include <utility>

inline static std::string generate_unique_string() {
    // 创建UUID对象
    boost::uuids::uuid uuid = boost::uuids::random_generator()();

    // 将UUID转换为字符串
    std::string unique_string = to_string(uuid);

    return unique_string;
}


Status StatusServiceImpl::GetChatServer(ServerContext *context, const GetChatServerReq *request, GetChatServerRsp *reply) {
    std::string prefix("mxchat status server has received :  ");
    spdlog::info("{}", prefix + request->DebugString());

    // 确保 _server_index 在有效范围内
    _server_index = (_server_index + 1) % _servers.size();

    auto &server = _servers[_server_index];

    // 记录选中的服务器
    spdlog::info("Selected server: host={}, port={}", server.host, server.port);

    reply->set_host(server.host);
    reply->set_port(server.port);
    reply->set_error(ErrorCodes::SUCCESSFUL);
    reply->set_token(generate_unique_string());

    _serverInfo.totalConnection += 1;

    // 更新对应服务器的连接数
    size_t server_id = _server_index;
    if (_serverInfo.bindAddressStatus.find(server_id) != _serverInfo.bindAddressStatus.end()) {
        _serverInfo.bindAddressStatus[server_id].second += 1; // 增加连接数
    } else {
        _serverInfo.bindAddressStatus[server_id] = {server.host, 1}; // 初始化连接数为1
    }

    notifyObservers(); // 通知观察者
    return Status::OK;
}
void StatusServiceImpl::addObserver(StatusMonitor *observer) {
    _observers.push_back(observer);
}

[[maybe_unused]]
void StatusServiceImpl::removeObserver(StatusMonitor *observer) {
    _observers.erase(std::remove(_observers.begin(), _observers.end(), observer), _observers.end());
}

void StatusServiceImpl::notifyObservers() {
    for (auto observer: _observers) {
        observer->UpdateServerStatus();
    }
}

StatusServiceImpl::StatusServiceImpl() : _server_index(0) {
    auto &cfg = Config_Manager::getInstance();
    ChatServer server;
    cfg.setYamlPath("server.yaml");

    // 服务器1
    server.port = cfg["ChatServer1"]["port"].as<std::string>();
    server.host = cfg["ChatServer1"]["host"].as<std::string>();
    _servers.push_back(server);
    spdlog::info("ChatSever1 host:{} port:{}", server.host, server.port);

    // 服务器2
    server.port = cfg["ChatServer2"]["port"].as<std::string>();
    server.host = cfg["ChatServer2"]["host"].as<std::string>();
    _servers.push_back(server);
    spdlog::info("ChatSever2 host:{} port:{}", server.host, server.port);

    // 初始化服务器状态
    _serverInfo = {};
    _serverInfo.totalConnection = 0;
    _serverInfo.bindAddressStatus.insert({0, {server.host, 0}}); // 服务器1的连接数初始化为0
    _serverInfo.bindAddressStatus.insert({1, {server.host, 0}}); // 服务器2的连接数初始化为0

    // 注册观察者
    addObserver(StatusMonitor::GetInstance(this));
}
const ServerStatus &StatusServiceImpl::serverInfo() const {
    return _serverInfo;
}
