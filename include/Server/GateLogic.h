/**
 * @FilePath     : /mxChat/include/Server/GateLogic.h
 * @Description  :  GateServer(Http)的Logic处理部分
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 17:57:03
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/


#pragma once
#include "DataBase/MysqlMgr.h"
#include "DataBase/RedisMgr.h"
#include "Server/HttpConnection.h"
#include "Server/StatusGrpcClient.hpp"
#include "Server/VarifyGrpcClient.h"
#include "Singleton.h"
#include <functional>
#include <map>
#include <spdlog/spdlog.h>

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

/**
 * @author       : caomengxuan
 * @brief        : GateServer的Logic处理单例类，处理来自客户端的Http/Https请求
**/
class GateLogic : public Singleton<GateLogic> {
    friend class Singleton<GateLogic>;

public:
    ~GateLogic();
    /**
	* @author       : caomengxuan
	* @brief        : 解析来自客户端的Get请求
	* @note 		: 从外部注册即可使用所有的Get请求
	* @param         {shared_ptr<HttpConnection>} con: 管理客户端的Http连接类的shared_ptr
	* @return        {bool} Get操作是否成功
    **/
    bool HandleGet(std::string url, std::shared_ptr<HttpConnection> con);
    /**
     * @author       : caomengxuan
     * @brief        : 动态注册Get请求,在GateLogic的构造函数中被使用
	 * @param         {string} url: 绑定的路由url
     * @param         {HttpHandler} handler: 路由对应的处理函数
     * @return        {*}
    **/
    void RegGet(std::string url, HttpHandler handler);
    /**
     * @author       : caomengxuan
     * @brief        : 动态注册Post请求,在GateLogic的构造函数中被使用
	 * @param         {string} url: 绑定的路由url
     * @param         {HttpHandler} handler: 路由对应的处理函数
     * @return        {*}
    **/
    void RegPost(std::string url, HttpHandler handler);
    /**
     * @author       : caomengxuan
     * @brief        : 解析来自客户端的Post请求
	 * @note 		 : 从外部注册即可使用所有的Get请求
     * @param         {HttpHandler} handler: 路由对应的处理函数
	 *  @param         {shared_ptr<HttpConnection>} con: 管理客户端的Http连接类的shared_ptr
     * @return        {bool} Post操作是否成功
    **/
    bool HandlePost(std::string, std::shared_ptr<HttpConnection> con);

private:
    /**
	 * @author       : caomengxuan
	 * @brief        : 在这里初始化所有的逻辑处理部分,如获取验证码和用户注册,重置密码,登录,所有解析后的操作在这里定义和实现
	 * @return        {*}
	**/
    GateLogic();
    std::map<std::string, HttpHandler> _post_handlers;
    std::map<std::string, HttpHandler> _get_handlers;
};
