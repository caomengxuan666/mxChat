/**
 * @FilePath     : /mxChat/src/ChatServer/GateLogic.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-02 22:29:55
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/


#pragma once
#include "DataBase/MysqlMgr.h"
#include "DataBase/RedisMgr.h"
#include "Server/HttpConnection.h"
#include "Server/StatusGrpcClient.hpp"
#include "Server/VarifyGrpcClient.h"
#include <spdlog/spdlog.h>
#include "Singleton.h"
#include <functional>
#include <map>

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;
class GateLogic :public Singleton<GateLogic>
{
	friend class Singleton<GateLogic>;
public:
	~GateLogic();
	bool HandleGet(std::string, std::shared_ptr<HttpConnection>);
	void RegGet(std::string, HttpHandler handler);
	void RegPost(std::string, HttpHandler handler);
	bool HandlePost(std::string, std::shared_ptr<HttpConnection>);
private:
GateLogic();
	std::map<std::string, HttpHandler> _post_handlers;
	std::map<std::string, HttpHandler> _get_handlers;
};
