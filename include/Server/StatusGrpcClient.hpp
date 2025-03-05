/**
 * @FilePath     : /mxChat/include/Server/StatusGrpcClient.hpp
 * @Description  :  grpc状态服务器的客户端
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 18:32:22
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include "message.pb.h"
#include <spdlog/spdlog.h>
#ifdef INTERNAL
#undef INTERNAL
#endif


#include "Singleton.h"
#include "config.hpp"
#include "const.h"
#include "message.grpc.pb.h"
#include <Server/StatusGrpcClient.hpp>
#include <grpcpp/create_channel.h>
#include <queue>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginReq;
using message::LoginRsp;
using message::StatusService;

/**
 * @author       : caomengxuan
 * @brief        : 状态服务器的线程池
 * @date         : grpc默认使用IPV6地址，这里使用的也是IPV6
**/
class StatusConPool {
public:
    /**
     * @author       : caomengxuan
     * @brief        : 创建grpc channel,并且构造的时候进行一次连接测试
     * @param         {size_t} poolSize:
     * @param         {string} port:
     * @return        {*}
    **/
    StatusConPool(size_t poolSize, std::string port)
        : poolSize_(poolSize), port_(port), b_stop_(false) {
        auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(100);
        for (size_t i = 0; i < poolSize_; ++i) {
            std::string target_address = "[::1]:";
            target_address += port;

            // 创建 gRPC Channel
            std::shared_ptr<Channel> channel = grpc::CreateChannel(target_address, grpc::InsecureChannelCredentials());
            if (i == 0) {
                // 测试 Channel 是否可用
                if (!channel->WaitForConnected(deadline)) {
                    spdlog::error("Failed to connect to {}", target_address);
                    throw std::runtime_error("Channel connection failed");
                }
            }
            // 将 Stub 放入连接池
            connections_.push(StatusService::NewStub(channel));
        }
    }

    /**
     * @author       : caomengxuan
     * @brief        : 析构函数，关闭连接池并清空连接队列
     * @return        {*}
    **/
    ~StatusConPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        Close();
        while (!connections_.empty()) {
            connections_.pop();
        }
    }

    /**
     * @author       : caomengxuan
     * @brief        : 获取一个连接
     * @return        {std::unique_ptr<StatusService::Stub>} :通过std::move返回客户端上下文对象
    **/    
    std::unique_ptr<StatusService::Stub> getConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] {
            if (b_stop_) {
                return true;
            }
            return !connections_.empty();
        });
        //如果停止则直接返回空指针
        if (b_stop_) {
            return nullptr;
        }
        auto context = std::move(connections_.front());
        connections_.pop();
        return context;
    }

    /**
     * @author       : caomengxuan
     * @brief        : 向状态服务器归还连接
     * @param         {unique_ptr<StatusService::Stub>} context:
     * @return        {*}
    **/    
    void returnConnection(std::unique_ptr<StatusService::Stub> context) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (b_stop_) {
            return;
        }
        connections_.push(std::move(context));
        cond_.notify_one();
    }

    /**
     * @author       : caomengxuan
     * @brief        : 关闭连接
     * @return        {*}
    **/    
    void Close() {
        b_stop_ = true;
        cond_.notify_all();
    }

private:
    std::atomic<bool> b_stop_;
    size_t poolSize_;
    std::string host_;
    std::string port_;
    std::queue<std::unique_ptr<StatusService::Stub>> connections_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

/**
 * @author       : caomengxuan
 * @brief        : 状态服务器,从配置文件中读取最大连接数,并且对所有ChatServer服务器进行负载均衡的连接管理
**/
class StatusGrpcClient : public Singleton<StatusGrpcClient> {
    friend class Singleton<StatusGrpcClient>;

public:
    ~StatusGrpcClient() {
    }
    /**
     * @author       : caomengxuan
     * @brief        : 通过uid,以grpc远程调用获取ChatServer服务器的连接
     * @param         {int} uid:
     * @return        {GetChatServerRsp} :grpc返回的ChatServer服务器信息
    **/    
    GetChatServerRsp GetChatServer(int uid) {
        ClientContext context;
        GetChatServerRsp reply;
        GetChatServerReq request;
        request.set_uid(uid);
        auto stub = pool_->getConnection();
        Status status = stub->GetChatServer(&context, request, &reply);

        // 使用 std::unique_ptr 和 lambda 表达式来替代 Defer
        auto defer = std::unique_ptr<void, std::function<void(void *)>>(nullptr, [&stub, this](void *) {
            pool_->returnConnection(std::move(stub));
        });
        if (status.ok()) {
            return reply;
        } else {
            reply.set_error(ErrorCodes::RPCGetFailed);
            spdlog::warn("RPC failed: {} \n detailed :{} \n", status.error_message(), status.error_details());
            return reply;
        }
    }

    /**
     * @author       : caomengxuan
     * @brief        : 通过uid,以grpc远程调用获取ChatServer服务器的连接
     * @param         {int} uid:
     * @param         {string} token:
     * @return        {*}
    **/    
    LoginRsp Login(int uid, std::string token) {
        ClientContext context;
        LoginRsp reply;
        LoginReq request;
        request.set_uid(uid);
        request.set_token(token);

        auto stub = pool_->getConnection();
        Status status = stub->Login(&context, request, &reply);

        // 使用 std::unique_ptr 和 lambda 表达式来替代 Defer
        auto defer = std::unique_ptr<void, std::function<void(void *)>>(nullptr, [&stub, this](void *) {
            pool_->returnConnection(std::move(stub));
        });
        if (status.ok()) {
            return reply;
        } else {
            reply.set_error(ErrorCodes::RPCGetFailed);
            return reply;
        }
    }

private:
    /**
     * @author       : caomengxuan
     * @brief        : 构造函数，读取配置文件,初始化连接池
     * @return        {*}
    **/
    StatusGrpcClient() {
        auto &cfg = Config_Manager::getInstance();
        cfg.setYamlPath("server.yaml");
        auto config = cfg.loadYamlDoc();
        //std::string host = gCfgMgr["StatusServer"]["Host"];
        std::string port = config["StatusServer"]["port"].as<std::string>();
        size_t pool_size = config["StatusServer"]["poolsize"].as<size_t>();
        spdlog::info("connect to status server at {}", port);
        spdlog::info("pool size is {}", pool_size);
        pool_.reset(new StatusConPool(pool_size, port));
    }

    std::unique_ptr<StatusConPool> pool_;
};
