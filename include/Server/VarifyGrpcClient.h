#pragma once

// 取消定义所有可能冲突的宏
#ifdef INTERNAL
# undef INTERNAL
#endif
#include "Singleton.h"
#include "config.hpp"
#include "const.h"
#include "message.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <memory>
#include <queue>
#include <spdlog/spdlog.h>
#include <yaml-cpp/node/node.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class RPConPool {
public:
    RPConPool(size_t poolSize, std::string host, std::string port)
        : poolSize_(poolSize), host_(host), port_(port), b_stop_(false) {
        for (size_t i = 0; i < poolSize_; ++i) {
            std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
                                                                   grpc::InsecureChannelCredentials());
            connections_.push(VarifyService::NewStub(channel));
        }
    }

    ~RPConPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        Close();
        while (!connections_.empty()) {
            connections_.pop();
        }
    }

    std::unique_ptr<VarifyService::Stub> getConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] {
            if (b_stop_) {
                return true;
            }
            return !connections_.empty();
        });
        if (b_stop_) {
            return nullptr;
        }
        auto context = std::move(connections_.front());
        connections_.pop();
        return context;
    }

    void returnConnection(std::unique_ptr<VarifyService::Stub> context) {
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
    std::queue<std::unique_ptr<VarifyService::Stub>> connections_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

class VerifyGrpcClient : public Singleton<VerifyGrpcClient> {
    friend class Singleton<VerifyGrpcClient>;

public:
    GetVarifyRsp GetVarifyCode(std::string email) {
        ClientContext context;
        GetVarifyRsp reply;
        GetVarifyReq request;
        request.set_email(email);
        auto stub = pool_->getConnection();
        Status status = stub->GetVarifyCode(&context, request, &reply);

        if (status.ok()) {
            pool_->returnConnection(std::move(stub));
            return reply;
        } else {
            pool_->returnConnection(std::move(stub));
            reply.set_error(ErrorCodes::RPCFailed);
            return reply;
        }
    }

    GetVarifyRsp SendResetPasswordCode(std::string email) {
        ClientContext context;
        GetVarifyRsp reply;
        GetVarifyReq request;
        request.set_email(email);
        auto stub = pool_->getConnection();
        Status status = stub->SendResetPasswordCode(&context, request, &reply);

        if (status.ok()) {
            pool_->returnConnection(std::move(stub));
            return reply;
        } else {
            pool_->returnConnection(std::move(stub));
            reply.set_error(ErrorCodes::RPCFailed);
            return reply;
        }
    }

private:
    VerifyGrpcClient() {
        try {
            // 获取 Config_Manager 单例实例
            auto &config_manager = Config_Manager::getInstance();

            // 设置并加载配置文件
            config_manager.setRootPath("/home/cmx/QtProjects/mxChat/config/");
            config_manager.setYamlPath("ServicePool.yaml");
            YAML::Node config = config_manager.loadYamlDoc();

            // 从配置文件中读取参数
            int poolSize = config["poolSize"].as<int>();
            std::string host = config["host"].as<std::string>();
            std::string port = config["port"].as<std::string>();

            // 初始化连接池
            pool_.reset(new RPConPool(poolSize, host, port));

            // 日志记录
            spdlog::info("VerifyGrpcClient initialized with poolSize: {}, host: {}, port: {}", poolSize, host, port);
        } catch (const YAML::Exception &yamlEx) {
            spdlog::critical("YAML parsing error: {}", yamlEx.what());
            assert(false); // 终止程序
        } catch (const std::exception &ex) {
            spdlog::critical("Failed to initialize VerifyGrpcClient: {}", ex.what());
            assert(false); // 终止程序
        }
    }

    std::unique_ptr<RPConPool> pool_;
};