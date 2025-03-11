/**
 * @FilePath     : /mxChat/include/Client/UserMgr.h
 * @Description  :  管理用户信息，负责为客户端和服务器端进行用户信息管理
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-04 23:31:36
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#ifndef USERMGR_H
#define USERMGR_H
#include <QObject>
#include <memory>
#include "Singleton.h"


/**
 * @author       : caomengxuan666
 * @brief        : 用户管理类，用于管理客户端发送到服务端的用户信息
**/
class UserMgr:public QObject,public Singleton<UserMgr>,
        public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
public:
    friend class Singleton<UserMgr>;
    ~ UserMgr();
    /**
     * @author       : caomengxuan666
     * @brief        : 设置用户名
     * @param         {QString} name:
     * @return        {*}
    **/    
    void SetName(QString name);
    /**
     * @author       : caomengxuan666
     * @brief        : 设置Uid
     * @param         {int} uid:
     * @return        {*}
    **/    
    void SetUid(int uid);
    /**
     * @author       : caomengxuan666
     * @brief        : 设置Token
     * @param         {QString} token:
     * @return        {*}
    **/    
    void SetToken(QString token);
private:
    UserMgr();
    QString _name;
    QString _token;
    int _uid;
};

#endif // USERMGR_H