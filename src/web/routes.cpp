#include "routes.hpp"
#include "database.hpp"
#include "message.pb.h"
#include "utils.hpp"// 包含 download.hpp 头文件
#include <crow/app.h>
#include <crow/http_response.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <spdlog/spdlog.h>
#include <string>
#include <Server/VarifyGrpcClient.h>
#include<DataBase/RedisMgr.h>


// 定义静态成员变量
crow::SimpleApp Server::app;
size_t Server::port = 11451;

void Server::setupRoutes() {
    // 一个基础 GET 路由,直接指向index
    CROW_ROUTE(app, "/")
    ([](const crow::request &, crow::response &res) {
        res.code = 302;// 302 Found (临时重定向)
        res.add_header("Location", "/index");
        res.end();
    });

    //POST 路由，用于接收 JSON 数据
    CROW_ROUTE(app, "/post").methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        try {
            auto json_data = crow::json::load(req.body);
            if (!json_data) {
                return crow::response(400, "Invalid JSON format");
            }
            std::string key = json_data["key"].s();
            return crow::response(200, "Received key: " + key);
        } catch (const std::exception &e) {
            return crow::response(500, "Error: " + std::string(e.what()));
        }
    });

    // 主页路由
    CROW_ROUTE(app, "/index")
    ([]() {
        std::string file_path = "templates/index.html";// 确保文件路径正确
        std::ifstream file(file_path, std::ios::in);
        if (!file.is_open()) {
            spdlog::error("Failed to open file: {}", file_path);
            return crow::response(500, "Failed to open file");
        }
        std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        crow::response res(200, str);
        res.add_header("Content-Type", "text/html");
        spdlog::info("Returning index.html with Content-Type: text/html");
        return res;
    });

    // 文件下载路由，默认流式传输
    CROW_ROUTE(app, "/download")
    ([]() {
        return utilFunc::downloadFile("Bin.zip");
    });


    // 重置密码路由
    CROW_ROUTE(app, "/reset-password").methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        try {
            // 打印请求头
            CROW_LOG_INFO << "Headers:";
            for (const auto &header: req.headers) {
                CROW_LOG_INFO << header.first << ": " << header.second;
            }

            // 打印请求体
            CROW_LOG_INFO << "Body: " << req.body;

            auto json_data = crow::json::load(req.body);

            if (!json_data) {
                CROW_LOG_ERROR << "Invalid JSON received";
                return crow::response(400, "Invalid JSON format");
            }
            // 打印解析后的 JSON
            CROW_LOG_INFO << "Parsed JSON: " << json_data;

            std::string email = json_data["email"].s();
            std::string username = json_data["username"].s();
            std::string newPassword = json_data["newPassword"].s();

            // 验证新密码的条件
            std::regex password_regex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*?&])[A-Za-z\\d@$!%*?&]{8,}$");
            if (!std::regex_match(newPassword, password_regex)) {
                return crow::response(400, "Password must be at least 8 characters long, contain uppercase and lowercase letters, numbers, and special characters.");
            }

            //根据邮箱，找回密码


            auto mysql_handler = MysqlHandler::GetInstance();
            auto res = mysql_handler->ResetPwdByEmail(email, newPassword);
            if (res == HandleResult::NO_SUCH_USER_OR_EMAIL)
                return crow::response(500, "No such user or email: " + email);
            else if (res == HandleResult::EXCEPTION)
                return crow::response(500, "Exception occurred while resetting password for " + email);
            else if (res == HandleResult::CONNECT_ERROR)
                return crow::response(500, "Connect error occurred while resetting password for " + email);

            return crow::response(200, "Password reset successful for " + email);
        } catch (const std::exception &e) {
            return crow::response(500, "Error: " + std::string(e.what()));
        }
    });

    // 重置密码页面路由
    CROW_ROUTE(app, "/reset-password.html")
    ([]() {
        std::string file_path = "templates/reset-password.html";// 确保文件路径正确
        std::ifstream file(file_path, std::ios::in);
        if (!file.is_open()) {
            spdlog::error("Failed to open file: {}", file_path);
            return crow::response(500, "Failed to open file");
        }
        std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        crow::response res(200, str);
        res.add_header("Content-Type", "text/html");
        spdlog::info("Returning reset-password.html with Content-Type: text/html");
        return res;
    });

    // 发送验证码路由
    CROW_ROUTE(app, "/send-verification-code").methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        try {
            auto json_data = crow::json::load(req.body);
            if (!json_data) {
                return crow::response(400, "Invalid JSON format");
            }
            std::string email = json_data["email"].s();

            // 调用 JavaScript 服务发送验证码
            // 这里假设你有一个函数可以调用 JavaScript 服务
            // 例如：sendVerificationCodeToEmail(email);
            // 你需要实现这个函数来调用 JavaScript 服务
            GetVarifyRsp rsp=VerifyGrpcClient::GetInstance()->GetVarifyCode(email);
    
            return crow::response(200, "Verification code sent to " + email);
        } catch (const std::exception &e) {
            return crow::response(500, "Error: " + std::string(e.what()));
        }
    });

    // 验证验证码路由
    CROW_ROUTE(app, "/verify-verification-code").methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        try {
            auto json_data = crow::json::load(req.body);
            if (!json_data) {
                return crow::response(400, "Invalid JSON format");
            }
            std::string email = json_data["email"].s();
            std::string verificationCode = json_data["verificationCode"].s();

            // 从 Redis 中读取验证码
            // 这里假设你有一个函数可以从 Redis 中读取验证码
            // 例如：std::string storedCode = getVerificationCodeFromRedis(email);
            // 你需要实现这个函数来从 Redis 中读取验证码

            // 假设 storedCode 是从 Redis 中读取到的验证码
            std::string storedCode = "123456";// 示例验证码

            if (storedCode == verificationCode) {
                return crow::response(200, "Verification code is correct");
            } else {
                return crow::response(400, "Verification code is incorrect");
            }
        } catch (const std::exception &e) {
            return crow::response(500, "Error: " + std::string(e.what()));
        }
    });

    // 捕获所有未定义路由
    CROW_ROUTE(app, "/<string>").methods(crow::HTTPMethod::GET)([](const std::string &path) {
        crow::response res(404, utilFunc::pageNotFound());
        return res;
    });
}

void Server::initCrow() {
    setupRoutes();// 调用 setupRoutes 方法

    // 启动 Crow 服务器
    try {
        spdlog::info("Crow server is running on http://127.0.0.1:{}", port);
        app.port(port).multithreaded().run();
    } catch (const std::exception &e) {
        std::cerr << "Failed to start server: " << e.what() << std::endl;
    }
}