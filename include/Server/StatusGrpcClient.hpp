/**
 * @FilePath     : /mxChat/include/Server/StatusGrpcClient.hpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-22 13:27:22
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

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
using message::StatusService;

class StatusConPool {
public:
    StatusConPool(size_t poolSize, std::string host, std::string port)
        : poolSize_(poolSize), host_(host), port_(port), b_stop_(false) {
        for (size_t i = 0; i < poolSize_; ++i) {

            std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
                                                                   grpc::InsecureChannelCredentials());

            connections_.push(StatusService::NewStub(channel));
        }
    }

    ~StatusConPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        Close();
        while (!connections_.empty()) {
            connections_.pop();
        }
    }

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

    void returnConnection(std::unique_ptr<StatusService::Stub> context) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (b_stop_) {
            return;
        }
        connections_.push(std::move(context));
        cond_.notify_one();
    }

    void Close() {
        b_stop_ = true;
        cond_.notify_all();
    }

private:
    atomic<bool> b_stop_;
    size_t poolSize_;
    std::string host_;
    std::string port_;
    std::queue<std::unique_ptr<StatusService::Stub>> connections_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

class StatusGrpcClient : public Singleton<StatusGrpcClient> {
    friend class Singleton<StatusGrpcClient>;

public:
    ~StatusGrpcClient() {
    }
    GetChatServerRsp GetChatServer(int uid){
        ClientContext context;
        GetChatServerRsp reply;
        GetChatServerReq request;
        request.set_uid(uid);
        auto stub = pool_->getConnection();
        Status status = stub->GetChatServer(&context, request, &reply);
    
        // 使用 std::unique_ptr 和 lambda 表达式来替代 Defer
        auto defer = std::unique_ptr<void, std::function<void(void*)>>(nullptr, [&stub, this](void*) {
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
    StatusGrpcClient(){
        auto &cfg = Config_Manager::getInstance();
        cfg.setYamlPath("server.yaml");
        auto config = cfg.loadYamlDoc();
        //std::string host = gCfgMgr["StatusServer"]["Host"];
        std::string host = config["StatusServer"]["host"].as<std::string>();
        std::string port = config["StatusServer"]["port"].as<std::string>();
        pool_.reset(new StatusConPool(5, host, port));
    }
    std::unique_ptr<StatusConPool> pool_;
};
