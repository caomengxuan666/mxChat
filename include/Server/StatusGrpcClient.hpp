/**
 * @FilePath     : /mxChat/include/Server/StatusGrpcClient.hpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-24 19:59:58
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

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
using message::StatusService;

class StatusConPool {
public:
    StatusConPool(size_t poolSize, std::string port)
        : poolSize_(poolSize), port_(port), b_stop_(false) {
        auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(100);
        for (size_t i = 0; i < poolSize_; ++i) {
            // 硬编码为 [::1]:8083
            std::string target_address = "[::1]:8083";

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

private:
    StatusGrpcClient() {
        auto &cfg = Config_Manager::getInstance();
        cfg.setYamlPath("server.yaml");
        auto config = cfg.loadYamlDoc();
        //std::string host = gCfgMgr["StatusServer"]["Host"];
        std::string port = config["StatusServer"]["port"].as<std::string>();
        spdlog::info("connect to status server at {}", port);
        pool_.reset(new StatusConPool(5, port));
    }
    std::unique_ptr<StatusConPool> pool_;
};
