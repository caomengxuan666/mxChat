/**
 * @FilePath     : /mxChat/include/UserMgr.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-01 17:25:39
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#ifndef USERMGR_H
#define USERMGR_H
#include <QObject>
#include <memory>
#include "Singleton.h"

class UserMgr:public QObject,public Singleton<UserMgr>,
        public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
public:
    friend class Singleton<UserMgr>;
    ~ UserMgr();
    void SetName(QString name);
    void SetUid(int uid);
    void SetToken(QString token);
private:
    UserMgr();
    QString _name;
    QString _token;
    int _uid;
};

#endif // USERMGR_H