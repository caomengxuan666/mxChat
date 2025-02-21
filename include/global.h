/**
 * @FilePath     : /mxChat/include/global.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-21 22:11:55
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once
#include <string>
enum ReqId {
    ID_GET_VARIFY_CODE = 1001,//获取验证码
    ID_REG_USER = 1002,       //注册用户
    ID_LOGIN_USER = 1003,     //登录用户
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