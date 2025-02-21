/**
 * @FilePath     : /mxChat/include/Server/const.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-21 22:43:15
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

enum ErrorCodes {
	SUCCESSFUL = 0,
	Error_Json = 1001,  //Json解析错误
	RPCGetFailed = 1002,  //RPC请求错误
	VarifyExpired=1003,
	VarifyCodeErr=1004,
	UserExist=1005,
	PasswdErr=1006,
	ERR_NETWORK=1007,
};