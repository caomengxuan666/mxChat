/**
 * @FilePath     : /mxChat/include/Server/UserMgr.h
 * @Description  :  用户的会话信息管理封装类
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 21:13:03
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once
#include "Singleton.h"
#include <memory>
#include <mutex>
#include <unordered_map>

class CSession;

/**
 * @author       : caomengxuan
 * @brief        : 
**/
class UserMgr : public Singleton<UserMgr> {
    friend class Singleton<UserMgr>;

public:
    ~UserMgr();
    /**
	* @author       : caomengxuan
	* @brief        : 通过uid获取用户session
	* @param         {int} uid:
	* @return        {std::shared_ptr<CSession>} 用户与服务器建立的的Session
	**/
    std::shared_ptr<CSession> GetSession(int uid);
    /**
	* @author       : caomengxuan
	* @brief        : 
	* @param         {int} uid:
	* @param         {shared_ptr<CSession>} session:
	* @return        {*}
	**/
    void SetUserSession(int uid, std::shared_ptr<CSession> session);
    /**
     * @author       : caomengxuan
     * @brief        : 删除用户session
     * @param         {int} uid:
     * @return        {*}
    **/
    void RmvUserSession(int uid);

private:
    UserMgr();
    std::mutex _session_mtx;
    std::unordered_map<int, std::shared_ptr<CSession>> _uid_to_session;
};
