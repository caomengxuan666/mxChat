/**
 * @FilePath     : /mxChat/include/global.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-04 11:49:55
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once
#include <string>

//todo

enum ReqId {
    ID_GET_VARIFY_CODE = 1001,//获取验证码
    ID_REG_USER = 1002,       //注册用户
    ID_LOGIN_USER = 1003,     //登录用户
    ID_CHAT_LOGIN=1005,        //聊天登录
    //MSG_CHAT_LOGIN=1005,        //用户登录
    //MSG_CHAT_LOGIN_RSP=1007,
    ID_CHAT_LOGIN_RSP=1008,     
};


enum Modules {
    REGISTERMOD = 0,
    RESETMOD = 1,
    LOGINMOD = 2
};

struct UserInfo {
    std::string name;
    std::string email;
    std::string pwd;
    size_t uid;
};