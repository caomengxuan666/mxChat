/**
 * @FilePath     : /mxChat/src/ChatServer/UserMgr.cpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 23:24:08
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include "Server/UserMgr.h"


UserMgr::UserMgr() {
    _uid_to_session.clear();
}

UserMgr::~UserMgr() {
}
void UserMgr::RmvUserSession(int uid) {
    auto uid_str = std::to_string(uid);
    //因为再次登录可能是其他服务器，所以会造成本服务器删除key，其他服务器注册key的情况
    // 有可能其他服务登录，本服删除key造成找不到key的情况
    //RedisMgr::GetInstance()->Del(USERIPPREFIX + uid_str);

    {
        std::lock_guard<std::mutex> lock(_session_mtx);
        _uid_to_session.erase(uid);
    }
}

void UserMgr::SetUserSession(int uid, std::shared_ptr<CSession> session) {
    std::lock_guard<std::mutex> lock(_session_mtx);
    _uid_to_session[uid] = session;
}