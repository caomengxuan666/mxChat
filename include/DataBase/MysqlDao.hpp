/**
 * @FilePath     : /mxChat/include/DataBase/MysqlDao.hpp
 * @Description  :  MySQL的查询实现层
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-04 23:42:17
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#ifndef MYSQL_DAO_HPP
#define MYSQL_DAO_HPP

#include "../Server/config.hpp"
#include "MySqlPool.hpp"
#include "global.h"
#include <string>

/**
 * @author       : caomengxuan
 * @brief        : 实现MySQL的具体操作
**/
class MysqlDao {
public:
    /**
     * @author       : caomengxuan
     * @brief        : MySQL的构造函数,读取yaml配置文档并且初始化连接池，自动连接到数据库
    **/
    MysqlDao();
    /**
     * @author       : caomengxuan
     * @brief        : MySQL的析构函数，进行MySQL连接池的回收
     * @return        {*}
    **/    
    ~MysqlDao();
    /**
     * @author       : caomengxuan
     * @brief        : 注册用户，将信息写入MySQL
     * @param         {string} &name:
     * @param         {string} &email:
     * @param         {string} &pwd:
     * @return        {int} resut 注册成功->uid 注册失败-> -1
    **/    
    int RegUser(const std::string &name, const std::string &email, const std::string &pwd);
    /**
     * @author       : caomengxuan
     * @brief        : 密码检测，返回一个检测的结果
     * @param         {string} &name:
     * @param         {string} &pwd:
     * @param         {UserInfo} &userInfo:
     * @return        {bool} 密码检测的结果
    **/    
    bool CheckPwd(const std::string &name, const std::string &pwd, UserInfo &userInfo);
    /**
     * @author       : caomengxuan
     * @brief        : 通过uid获取用户所有信息
     * @param         {int} uid:
     * @return        {shared_ptr<_NonArray<UserInfo>>} 相关用户在数据库中所有的信息
    **/    
    std::shared_ptr<UserInfo> GetUser(int uid);
    /**
    * @author       : caomengxuan
    * @brief        : 通过name获取用户所有信息
    * @param         {string} name:
    * @return        {shared_ptr<_NonArray<UserInfo>>} 相关用户在数据库中所有的信息
    **/    
	std::shared_ptr<UserInfo> GetUser(std::string name);
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

inline bool MysqlDao::CheckPwd(const std::string &name, const std::string &pwd, UserInfo &userInfo) {
    auto session = pool_->getConnection();
    if (!session) {
        std::cerr << "Failed to get connection from pool" << std::endl;
        return false;
    }

    try {
        // 调用存储过程 check_pwd
        auto result = session->sql("CALL check_pwd(?, ?, @result)")
                              .bind(name)
                              .bind(pwd)
                              .execute();

        // 获取存储过程的输出参数 @result
        auto res = session->sql("SELECT @result AS result").execute();
        auto row = res.fetchOne();

        if (row) {
            int result = row[0]; // 获取输出参数值,id
            std::cout << "Result: " << result << std::endl;

            if (result == 1) {
                // 获取用户信息
                auto userRes = session->sql("SELECT * FROM user WHERE name = ?")
                                        .bind(name)
                                        .execute();
                auto userRow = userRes.fetchOne();

                if (userRow) {
                    userInfo.uid = userRow[1].get<int>();
                    userInfo.name = userRow[2].get<std::string>(); 
                    userInfo.email = userRow[3].get<std::string>();
                    userInfo.pwd = userRow[4].get<std::string>();
                }
            }

            // 归还连接
            pool_->returnConnection(session);
            return result == 1;
        }

        // 如果没有结果，返回错误
        pool_->returnConnection(session);
        return false;
    } catch (const mysqlx::Error &e) {
        // 捕获异常并归还连接
        pool_->returnConnection(session);
        std::cerr << "SQLException: " << e.what() << std::endl;
        return false;
    }
}
inline std::shared_ptr<UserInfo> MysqlDao::GetUser(int uid) {
    auto session = pool_->getConnection();
    if (!session) {
        std::cerr << "Failed to get connection from pool" << std::endl;
        return nullptr;
    }

    // 使用 std::unique_ptr 和 lambda 表达式来替代 Defer
    auto defer = std::unique_ptr<void, std::function<void(void *)>>(nullptr, [this, &session](void *) {
        pool_->returnConnection(session);
    });

    try {
        // 准备SQL语句
        auto result = session->sql("SELECT uid, name, email, pwd FROM user WHERE uid = ?")
                              .bind(uid)
                              .execute();

        // 获取单行结果
        auto row = result.fetchOne();
        if (!row) {
            std::cerr << "No user found with uid: " << uid << std::endl;
            return nullptr;
        }
        // 构造 UserInfo 对象
        auto user_ptr = std::make_shared<UserInfo>();
        user_ptr->uid = row[0].get<int>();         // 第一个字段：uid
        user_ptr->name = row[1].get<std::string>(); // 第二个字段：name
        user_ptr->email = row[2].get<std::string>();// 第三个字段：email
        user_ptr->pwd = row[3].get<std::string>();  // 第四个字段：pwd

        return user_ptr;
    } catch (const mysqlx::Error &e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.what();
        std::cerr << ", SQLState: " << e.what() << " )" << std::endl;
        return nullptr;
    }
}

inline std::shared_ptr<UserInfo> MysqlDao::GetUser(std::string name) {
    auto session = pool_->getConnection();
    if (!session) {
        std::cerr << "Failed to get connection from pool" << std::endl;
        return nullptr;
    }

    // 使用 std::unique_ptr 和 lambda 表达式来替代 Defer
    auto defer = std::unique_ptr<void, std::function<void(void *)>>(nullptr, [this, &session](void *) {
        pool_->returnConnection(session);
    });

    try {
        // 准备SQL语句
        auto result = session->sql("SELECT uid, name, email, pwd FROM user WHERE name = ?")
                              .bind(name)
                              .execute();

        // 获取单行结果
        auto row = result.fetchOne();
        if (!row) {
            std::cerr << "No user found with name: " << name << std::endl;
            return nullptr;
        }

        // 构造 UserInfo 对象
        auto user_ptr = std::make_shared<UserInfo>();
        user_ptr->uid = row[0].get<int>();         // 第一个字段：uid
        user_ptr->name = row[1].get<std::string>(); // 第二个字段：name
        user_ptr->email = row[2].get<std::string>();// 第三个字段：email
        user_ptr->pwd = row[3].get<std::string>();  // 第四个字段：pwd

        return user_ptr;
    } catch (const mysqlx::Error &e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.what();
        std::cerr << ", SQLState: " << e.what() << " )" << std::endl;
        return nullptr;
    }
}
#endif// MYSQL_DAO_HPP
