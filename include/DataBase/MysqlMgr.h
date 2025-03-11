/**
 * @FilePath     : /mxChat/include/DataBase/MysqlMgr.h
 * @Description  :  mysql的高层封装层，将具体实现封装起来，通过新的外观接口适配给服务器层调用。
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 16:48:40
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once


#include "MysqlDao.hpp"
#include "Singleton.h"
#include "global.h"

/**
 * @author       : caomengxuan666
 * @brief        : Mysql的封装层，通过封装具体实现类MysqlDao,将具体接口暴露给服务器使用
**/
class MysqlMgr : public Singleton<MysqlMgr> {
    friend class Singleton<MysqlMgr>;

public:
    ~MysqlMgr();
    /**
     * @author       : caomengxuan666
     * @brief        : 
     * @param         {string} &name:
     * @param         {string} &email:
     * @param         {string} &pwd:
     * @return        {int} resut 注册成功->uid 注册失败-> -1
    **/
    int RegUser(const std::string &name, const std::string &email, const std::string &pwd);
    /**
     * @author       : caomengxuan666
     * @brief        : 
     * @param         {string} &name:
     * @param         {string} &pwd:
     * @param         {UserInfo} &userinfo:
     * @return        {bool} 密码检测的结果
    **/
    bool CheckPwd(const std::string &name, const std::string &pwd, UserInfo &userinfo);
    /**
     * @author       : caomengxuan666
     * @brief        : 通过uid获取用户所有信息
     * @param         {int} uid:
     * @return        {shared_ptr<_NonArray<UserInfo>>} 相关用户在数据库中所有的信息
    **/
    std::shared_ptr<UserInfo> GetUser(int uid);
    /**
    * @author       : caomengxuan666
    * @brief        : 通过name获取用户所有信息
    * @param         {string} name:
    * @return        {shared_ptr<UserInfo>} 相关用户在数据库中所有的信息
    **/
    std::shared_ptr<UserInfo> GetUser(std::string name);

private:
    MysqlMgr();
    MysqlDao _dao;
};