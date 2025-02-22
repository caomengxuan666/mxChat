/**
 * @FilePath     : /mxChat/src/ChatServer/StatusServiceImpl.cpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-22 13:29:24
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include <Server/StatusServiceImpl.h>
#include <Server/config.hpp>
#include <Server/const.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

inline static std::string generate_unique_string() {
    // 创建UUID对象
    boost::uuids::uuid uuid = boost::uuids::random_generator()();

    // 将UUID转换为字符串
    std::string unique_string = to_string(uuid);

    return unique_string;
}

Status StatusServiceImpl::GetChatServer(ServerContext *context, const GetChatServerReq *request, GetChatServerRsp *reply) {
    std::string prefix("llfc status server has received :  ");
    _server_index = (_server_index++) % (_servers.size());
    auto &server = _servers[_server_index];
    reply->set_host(server.host);
    reply->set_port(server.port);
    reply->set_error(ErrorCodes::SUCCESSFUL);
    reply->set_token(generate_unique_string());
    return Status::OK;
}

StatusServiceImpl::StatusServiceImpl() : _server_index(0) {
    auto &cfg = Config_Manager::getInstance();
    ChatServer server;
    cfg.setYamlPath("server.yaml");
    server.port = cfg["ChatServer1"]["port"].as<std::string>();
    server.host = cfg["ChatServer1"]["host"].as<std::string>();
    _servers.push_back(server);

    server.port = cfg["ChatServer2"]["port"].as<std::string>();
    server.host = cfg["ChatServer2"]["host"].as<std::string>();
    _servers.push_back(server);
}