/**
 * @FilePath     : /mxChat/src/ChatServer/MysqlMgr.cpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-21 22:12:37
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include "DataBase/MysqlMgr.h"


MysqlMgr::~MysqlMgr() {

}

int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
	return _dao.RegUser(name, email, pwd);
}


bool MysqlMgr::CheckPwd(const std::string &name, const std::string &pwd, UserInfo &userinfo) {
    return _dao.CheckPwd(name, pwd, userinfo);
}
MysqlMgr::MysqlMgr() {
}