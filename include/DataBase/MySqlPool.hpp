/**
 * @FilePath     : /mxChat/include/DataBase/MySqlPool.hpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-18 23:35:15
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#ifndef MYSQLPOOL_HPP
#define MYSQLPOOL_HPP

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <mysqlx/xdevapi.h>
#include <queue>
#include <spdlog/spdlog.h>
#include <string>

class MySqlPool {
public:
    // 构造函数
    MySqlPool(const std::string &host, int port, const std::string &user, const std::string &pass, const std::string &schema, int poolSize)
        : host_(host), port_(port), user_(user), pass_(pass), schema_(schema), poolSize_(poolSize), b_stop_(false) {
        try {
            for (int i = 0; i < poolSize_; ++i) {
                mysqlx::Session session(host_, port_, user_, pass_);
                session.sql("USE " + schema_).execute();// 设置默认 schema
                pool_.push(std::make_shared<mysqlx::Session>(std::move(session)));
            }
            spdlog::info("mysql session pool init success");
        } catch (const mysqlx::Error &e) {
            // 处理异常
            std::cerr << "MySQL Pool Initialization Failed: " << e.what() << std::endl;
            //打印所有字段的信息
            spdlog::critical("[FAILED WITH Params] : hosts {}  port {} password {} schema {}", host, port, pass, schema);
        }
    }

    // 获取连接
    std::shared_ptr<mysqlx::Session> getConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] {
            if (b_stop_) {
                return true;
            }
            return !pool_.empty();
        });
        if (b_stop_) {
            return nullptr;
        }
        std::shared_ptr<mysqlx::Session> session(pool_.front());
        pool_.pop();
        return session;
    }

    // 归还连接
    void returnConnection(std::shared_ptr<mysqlx::Session> session) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (b_stop_) {
            return;
        }
        pool_.push(session);
        cond_.notify_one();
    }

    // 关闭连接池
    void Close() {
        b_stop_ = true;
        cond_.notify_all();
    }

    // 析构函数
    ~MySqlPool() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!pool_.empty()) {
            pool_.pop();
        }
    }

private:
    std::string host_;                                 // 数据库主机地址
    int port_;                                         // 数据库端口
    std::string user_;                                 // 数据库用户名
    std::string pass_;                                 // 数据库密码
    std::string schema_;                               // 数据库 schema
    int poolSize_;                                     // 连接池大小
    std::queue<std::shared_ptr<mysqlx::Session>> pool_;// 连接队列
    std::mutex mutex_;                                 // 互斥锁
    std::condition_variable cond_;                     // 条件变量
    std::atomic<bool> b_stop_;                         // 停止标志
};

#endif// MYSQLPOOL_HPP