/**
 * @FilePath     : /mxChat/src/web/routes.hpp
 * @Description  :
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-19 22:17:49
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
 **/
#pragma once
#include <crow.h>

class Server {
public:
    // 获取 Server 实例
    static Server &getInstance() {
        static Server instance;
        return instance;
    }

    void initCrow();
    void setupRoutes();// 添加 setupRoutes 方法的声明

private:
    Server() {}                                // 私有构造函数，防止外部实例化
    Server(const Server &) = delete;           // 删除拷贝构造函数
    Server &operator=(const Server &) = delete;// 删除赋值操作符

    static crow::SimpleApp app;
    static size_t port;
};
