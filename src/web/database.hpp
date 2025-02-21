/**
 * @FilePath     : /mxChat/src/web/database.hpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-20 22:18:00
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include "Singleton.h"
#include <DataBase/MySqlPool.hpp>
#include <Server/config.hpp>
#include <functional>
#include <mysqlx/devapi/common.h>
#include <spdlog/spdlog.h>

enum HandleResult {
    SUCCESS = 0,
    NO_SUCH_USER_OR_EMAIL = 1,
    EXCEPTION = 2,
    CONNECT_ERROR = 3
};

class MysqlHandler : public Singleton<MysqlHandler> {
    friend class Singleton<MysqlHandler>;
    using Operation = std::function<HandleResult(std::shared_ptr<mysqlx::Session>)>;

public:
    ~MysqlHandler() {
        pool_->Close();
    }

    HandleResult ResetPwdByEmail(const std::string &email, const std::string &pwd);
    HandleResult ResetPwdByUserName(const std::string &username, const std::string &pwd);

private:
    MysqlHandler();
    HandleResult operation(Operation operation) {
        auto session = pool_->getConnection();
        if (!session) {
            spdlog::error("mysql pool get connection error");
            return CONNECT_ERROR;
        }
        try {
            return operation(session);
        } catch (const mysqlx::Error &e) {
            spdlog::error("mysql query error:{}", e.what());
            return EXCEPTION;
        }
    }

    HandleResult QueryByUserName(const std::string &username);
    HandleResult QueryByEmail(const std::string &email);

    std::unique_ptr<MySqlPool> pool_;
};

inline MysqlHandler::MysqlHandler() {
    auto cfg = Config_Manager::getInstance();
    cfg.setRootPath("/home/cmx/QtProjects/mxChat/config/");
    cfg.setYamlPath("database.yaml");
    auto config = cfg.loadYamlDoc();

    try {
        // 检查 mysql 配置是否存在
        if (!config["mysql"]) {
            throw std::runtime_error("Missing 'mysql' section in YAML file");
        }

        // 打印整个 mysql 配置内容（调试用）
        //std::cout << "MySQL Config: " << config["mysql"] << std::endl;

        // 解析 host 字段
        const auto host = config["mysql"]["host"].as<std::string>();
        //std::cout << "Parsed Host: " << host << ", Type: " << typeid(host).name() << std::endl;

        // 解析 port 字段
        const auto port = config["mysql"]["port"].as<int>();
        //std::cout << "Parsed Port: " << port << ", Type: " << typeid(port).name() << std::endl;

        // 解析 user 字段
        const auto user = config["mysql"]["user"].as<std::string>();
        //std::cout << "Parsed User: " << user << ", Type: " << typeid(user).name() << std::endl;

        // 解析 password 字段
        const auto pwd = config["mysql"]["password"].as<std::string>();
        //std::cout << "Parsed Password: " << pwd << ", Type: " << typeid(pwd).name() << std::endl;

        // 解析 schema 字段
        const auto schema = config["mysql"]["schema"].as<std::string>();
        //std::cout << "Parsed Schema: " << schema << ", Type: " << typeid(schema).name() << std::endl;

        // 初始化连接池
        pool_.reset(new MySqlPool(host, port, user, pwd, schema, 5));
    } catch (const YAML::Exception &e) {
        std::cerr << "YAML parsing error: " << e.what() << std::endl;
        throw;
    } catch (const std::exception &e) {
        std::cerr << "Error initializing MySQL: " << e.what() << std::endl;
        throw;
    }
}

inline HandleResult MysqlHandler::QueryByUserName(const std::string &username) {
    return operation([username](std::shared_ptr<mysqlx::Session> session) {
        auto result = session->sql("select * from user where name=?")
                              .bind(username)
                              .execute();
        auto row = result.fetchOne();
        if (!row) return NO_SUCH_USER_OR_EMAIL;
        try {
            auto res = row.get(0).get<std::string>();
            spdlog::info("mysql query result:{}", res);
            return SUCCESS;
        } catch (const mysqlx::Error &e) {
            spdlog::error("mysql query error:{}", e.what());
            return EXCEPTION;
        }
    });
}

inline HandleResult MysqlHandler::QueryByEmail(const std::string &email) {
    return operation([email](std::shared_ptr<mysqlx::Session> session) {
        auto result = session->sql("select email from user where email=?")
                              .bind(email)
                              .execute();
        auto row = result.fetchOne();
        if (!row) return NO_SUCH_USER_OR_EMAIL;
        try {
            if (!row[0].isNull()) {
                auto res = row[0].get<std::string>();
                spdlog::info("mysql query result:{}", res);
                return SUCCESS;
            } else {
                spdlog::error("Query result is NULL");
                return EXCEPTION;
            }
        } catch (const mysqlx::Error &e) {
            spdlog::error("mysql query error:{}", e.what());
            return EXCEPTION;
        }
    });
}
inline HandleResult MysqlHandler::ResetPwdByEmail(const std::string &email, const std::string &pwd) {
    return operation([pwd, this, &email](std::shared_ptr<mysqlx::Session> session) {
        // 检查用户是否存在
        auto isExist = QueryByEmail(email);
        if (isExist != SUCCESS)
            return NO_SUCH_USER_OR_EMAIL;
        
        // 执行密码更新操作
        auto result = session->sql("update user set pwd=? where email=?")
                              .bind(pwd, email)
                              .execute();
        
        // 验证密码是否更新成功
        auto verifyResult = session->sql("select pwd from user where email=?")
                              .bind(email)
                              .execute();
        auto row = verifyResult.fetchOne();
        if (!row || row.get(0).get<std::string>() != pwd) {
            spdlog::error("Failed to update password for email: {}", email);
            return EXCEPTION;
        }
        
        spdlog::info("Password updated successfully for email: {}", email);
        return SUCCESS;
    });
}

inline HandleResult MysqlHandler::ResetPwdByUserName(const std::string &username, const std::string &pwd) {
    return operation([pwd, this, &username](std::shared_ptr<mysqlx::Session> session) {
        // 检查用户是否存在
        auto isExist = QueryByUserName(username);
        if (isExist != SUCCESS)
            return NO_SUCH_USER_OR_EMAIL;
        
        // 执行密码更新操作
        auto result = session->sql("update user set pwd=? where name=?")
                              .bind(pwd, username)
                              .execute();
        
        // 验证密码是否更新成功
        auto verifyResult = session->sql("select pwd from user where name=?")
                              .bind(username)
                              .execute();
        auto row = verifyResult.fetchOne();
        if (!row || row.get(0).get<std::string>() != pwd) {
            spdlog::error("Failed to update password for username: {}", username);
            return EXCEPTION;
        }
        
        spdlog::info("Password updated successfully for username: {}", username);
        return SUCCESS;
    });
}
