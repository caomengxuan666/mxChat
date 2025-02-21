/**
 * @FilePath     : /mxChat/include/DataBase/MysqlMgr.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-21 22:10:48
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include "../Server/const.h"

#include "MysqlDao.hpp"
#include "Singleton.h"
#include "global.h"
class MysqlMgr : public Singleton<MysqlMgr> {
    friend class Singleton<MysqlMgr>;

public:
    ~MysqlMgr();
    int RegUser(const std::string &name, const std::string &email, const std::string &pwd);
    bool CheckPwd(const std::string &name, const std::string &pwd, UserInfo &userinfo);

private:
    MysqlMgr();
    MysqlDao _dao;
};