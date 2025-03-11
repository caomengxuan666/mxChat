/**
 * @FilePath     : /mxChat/include/Server/VarifyGrpcClient.h
 * @Description  :  验证邮箱的grpc调用方
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 21:19:19
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

// 取消定义所有可能冲突的宏
#ifdef INTERNAL
#undef INTERNAL
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

/**
 * @author       : caomengxuan666
 * @brief        : grpc邮件发送客户端的RPC连接池
**/
class RPConPool {
public:
    /**
     * @author       : caomengxuan666
     * @brief        : 构造函数,创造和js的grpc服务器的channel
     * @param         {size_t} poolSize:
     * @param         {string} host:
     * @param         {string} port:
     * @return        {*}
    **/
    RPConPool(size_t poolSize, std::string host, std::string port)
        : poolSize_(poolSize), host_(host), port_(port), b_stop_(false) {
        for (size_t i = 0; i < poolSize_; ++i) {
            std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
                                                                   grpc::InsecureChannelCredentials());
            connections_.push(VarifyService::NewStub(channel));
        }
    }

    /**
     * @author       : caomengxuan666
     * @brief        : 析构函数,关闭连接池
     * @return        {*}
    **/
    ~RPConPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        Close();
        while (!connections_.empty()) {
            connections_.pop();
        }
    }

    /**
     * @author       : caomengxuan666
     * @brief        : 获取一个调用远程邮件发送服务的代理对象
     * @return        {std::unique_ptr<VarifyService::Stub>} :一个调用远程邮件发送服务的代理对象
    **/
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

    /**
     * @author       : caomengxuan666
     * @brief        : 将一个调用远程邮件发送服务的代理对象放回连接池
     * @param         {unique_ptr<VarifyService::Stub>} context:
     * @return        {*}
    **/
    void returnConnection(std::unique_ptr<VarifyService::Stub> context) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (b_stop_) {
            return;
        }
        connections_.push(std::move(context));
        cond_.notify_one();
    }

    /**
     * @author       : caomengxuan666
     * @brief        : 关闭连接池
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
    std::queue<std::unique_ptr<VarifyService::Stub>> connections_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

/**
 * @author       : caomengxuan666
 * @brief        : 验证邮箱的grpc调用方
**/
class VerifyGrpcClient : public Singleton<VerifyGrpcClient> {
    friend class Singleton<VerifyGrpcClient>;

public:
    /**
     * @author       : caomengxuan666
     * @brief        : 获取验证码的远程调用
     * @param         {string} email:
     * @return        {GetVarifyRsp}:验证码的reply返回
    **/
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
            reply.set_error(ErrorCodes::RPCGetFailed);
            return reply;
        }
    }

    /**
     * @author       : caomengxuan666
     * @brief        : 重置密码的远程调用
     * @param         {string} email:
     * @return        {GetVarifyRsp}:重置密码的reply返回
    **/
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
            reply.set_error(ErrorCodes::RPCGetFailed);
            return reply;
        }
    }

private:
    /**
     * @author       : caomengxuan666
     * @brief        : 构造函数，加载配置文件并且初始化连接池
     * @return        {*}
    **/
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
            assert(false);// 终止程序
        } catch (const std::exception &ex) {
            spdlog::critical("Failed to initialize VerifyGrpcClient: {}", ex.what());
            assert(false);// 终止程序
        }
    }

    std::unique_ptr<RPConPool> pool_;
};