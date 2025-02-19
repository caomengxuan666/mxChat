/**
 * @FilePath     : /mxChat/src/ChatServer/MysqlMgr.cpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-17 21:01:31
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include "DataBase/MysqlMgr.h"


MysqlMgr::~MysqlMgr() {

}

int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
	return _dao.RegUser(name, email, pwd);
}

MysqlMgr::MysqlMgr() {
}