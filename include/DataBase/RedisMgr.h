/**
 * @FilePath     : /mxChat/include/DataBase/RedisMgr.h
 * @Description  :  Redis的操作封装层
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 17:10:21
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once
#include "Singleton.h"

#include <spdlog/spdlog.h>
#include <sw/redis++/redis++.h>


constexpr std::string CODE_PREFIX = "code_";
/**
 * @author       : caomengxuan
 * @brief        : Redis的操作封装层
**/
class RedisMgr : public Singleton<RedisMgr>,
                 public std::enable_shared_from_this<RedisMgr> {
    friend class Singleton<RedisMgr>;

public:
    ~RedisMgr() {}
    /**
     * @author       : caomengxuan
     * @brief        : 连接redis服务器
     * @param         {string} &host:
     * @param         {int} port:
     * @return        {*}
    **/    
    bool Connect(const std::string &host, int port);
    /**
     * @author       : caomengxuan
     * @brief        : 获取redis服务器中的数据
     * @param         {string} &key:
     * @param         {string} &value:
     * @param         {string} &code_prefix:
     * @return        {*}
    **/    
    bool Get(const std::string &key, std::string &value, const std::string &code_prefix = CODE_PREFIX);
    /**
     * @author       : caomengxuan
     * @brief        : 设置redis中的数据
     * @param         {string} &key:
     * @param         {string} &value:
     * @return        {*}
    **/    
    bool Set(const std::string &key, const std::string &value);
    /**
     * @author       : caomengxuan
     * @brief        : 验证redis数据库的密码
     * @param         {string} &password:
     * @return        {*}
    **/    
    bool Auth(const std::string &password);
    /**
     * @author       : caomengxuan
     * @brief        : 队列操作向左Push
     * @param         {string} &key:
     * @param         {string} &value:
     * @return        {*}
    **/    
    bool LPush(const std::string &key, const std::string &value);
    /**
     * @author       : caomengxuan
     * @brief        : 队列操作向左Pop
     * @param         {string} &key:
     * @param         {string} &value:
     * @return        {*}
    **/
    bool LPop(const std::string &key, std::string &value);
    /**
     * @author       : caomengxuan
     * @brief        : 
     * @param         {string} &key:
     * @param         {string} &value:
     * @return        {*}
    **/    
    bool RPush(const std::string &key, const std::string &value);
    /**
     * @author       : caomengxuan
     * @brief        : 队列操作向右Pop
     * @param         {string} &key:
     * @param         {string} &value:
     * @return        {*}
    **/    
    bool RPop(const std::string &key, std::string &value);
    /**
     * @author       : caomengxuan
     * @brief        : Hash操作，设置
     * @param         {string} &key:
     * @param         {string} &hkey:
     * @param         {string} &value:
     * @return        {*}
    **/    
    bool HSet(const std::string &key, const std::string &hkey, const std::string &value);
    /**
     * @author       : caomengxuan
     * @brief        : Hash操作，设置
     * @param         {char} *key:
     * @param         {char} *hkey:
     * @param         {char} *hvalue:
     * @param         {size_t} hvaluelen:
     * @return        {*}
    **/    
    bool HSet(const char *key, const char *hkey, const char *hvalue, size_t hvaluelen);
    /**
     * @author       : caomengxuan
     * @brief        : Hash操作，获取
     * @param         {string} &key:
     * @param         {string} &hkey:
     * @return        {*}
    **/    
    std::string HGet(const std::string &key, const std::string &hkey);
    /**
     * @author       : caomengxuan
     * @brief        : 删除Redis中的键
     * @param         {string} &key:
     * @return        {*}
    **/    
    bool Del(const std::string &key);
    /**
     * @author       : caomengxuan
     * @brief        : 判断键是否存在
     * @param         {string} &key:
     * @return        {*}
    **/    
    bool ExistsKey(const std::string &key);
    /**
     * @author       : caomengxuan
     * @brief        : 关闭Redis连接
     * @return        {*}
    **/    
    void Close();

private:
    /**
     * @author       : caomengxuan
     * @brief        : 构造函数，构造时进行连接
     * @return        {*}
    **/
    RedisMgr();

    redisContext *_connect;
    redisReply *_reply;
};