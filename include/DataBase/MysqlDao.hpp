/**
 * @FilePath     : /mxChat/include/DataBase/MysqlDao.hpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-18 23:18:29
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#ifndef MYSQL_DAO_HPP
#define MYSQL_DAO_HPP

#include "../Server/config.hpp"
#include "MySqlPool.hpp"
#include <string>

class MysqlDao {
public:
    MysqlDao();
    ~MysqlDao();
    int RegUser(const std::string &name, const std::string &email, const std::string &pwd);

private:
    std::unique_ptr<MySqlPool> pool_;
};

inline MysqlDao::MysqlDao() {
    auto cfg = Config_Manager::getInstance();
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
        std::cerr << "Error initializing MySQL DAO: " << e.what() << std::endl;
        throw;
    }
}

inline MysqlDao::~MysqlDao() {
    pool_->Close();
}

inline int MysqlDao::RegUser(const std::string &name, const std::string &email, const std::string &pwd) {
    // 获取数据库会话
    auto session = pool_->getConnection();
    if (!session) {
        std::cerr << "Failed to get connection from pool" << std::endl;
        return -1;
    }

    try {
        // 调用存储过程 reg_user
        auto result = session->sql("CALL reg_user(?, ?, ?, @result)")
                              .bind(name)
                              .bind(email)
                              .bind(pwd)
                              .execute();

        // 获取存储过程的输出参数 @result
        auto res = session->sql("SELECT @result AS result").execute();
        auto row = res.fetchOne();

        if (row) {
            int result = row[0];// 获取输出参数值
            std::cout << "Result: " << result << std::endl;

            // 归还连接
            pool_->returnConnection(session);
            return result;
        }

        // 如果没有结果，返回错误
        pool_->returnConnection(session);
        return -1;
    } catch (const mysqlx::Error &e) {
        // 捕获异常并归还连接
        pool_->returnConnection(session);
        std::cerr << "SQLException: " << e.what() << std::endl;
        return -1;
    }
}

#endif// MYSQL_DAO_HPP
