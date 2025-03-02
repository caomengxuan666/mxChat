/**
 * @FilePath     : /mxChat/include/DataBase/MysqlMgr.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-02 11:21:06
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once


#include "MysqlDao.hpp"
#include "Singleton.h"
#include "global.h"
class MysqlMgr : public Singleton<MysqlMgr> {
    friend class Singleton<MysqlMgr>;

public:
    ~MysqlMgr();
    int RegUser(const std::string &name, const std::string &email, const std::string &pwd);
    bool CheckPwd(const std::string &name, const std::string &pwd, UserInfo &userinfo);
	std::shared_ptr<UserInfo> GetUser(int uid);
	std::shared_ptr<UserInfo> GetUser(std::string name);
private:
    MysqlMgr();
    MysqlDao _dao;
};