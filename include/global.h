/**
 * @FilePath     : /mxChat/include/global.h
 * @Description  :  全局定义，服务器和客户端共享
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-14 14:23:13
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once
#include <string>

//todo
/**
 * @author       : caomengxuan666
 * @brief        : 请求的ID
**/
enum ReqId {
    ID_GET_VARIFY_CODE = 1001,//获取验证码
    ID_REG_USER = 1002,       //注册用户
    ID_LOGIN_USER = 1003,     //登录用户
    ID_CHAT_LOGIN_RSP=1004,     //聊天登录的响应
    ID_CHAT_LOGIN=1005,        //聊天登录

};

/**
 * @author       : caomengxuan666
 * @brief        : 请求的模式
**/
enum Modules {
    REGISTERMOD = 0,  
    RESETMOD = 1,
    LOGINMOD = 2
};

/**
 * @author       : caomengxuan666
 * @brief        : 通信中的用户信息结构体
**/
struct UserInfo {
    std::string name;
    std::string email;
    std::string pwd;
    size_t uid;
};