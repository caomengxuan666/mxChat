/**
 * @FilePath     : /mxChat/src/ChatClient/UserMgr.cpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-01 17:26:36
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include<Client/UserMgr.h>

UserMgr::~UserMgr()
{

}

void UserMgr::SetName(QString name)
{
    _name = name;
}

void UserMgr::SetUid(int uid)
{
    _uid = uid;
}

void UserMgr::SetToken(QString token)
{
    _token = token;
}

UserMgr::UserMgr()
{

}