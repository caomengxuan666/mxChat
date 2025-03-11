/**
 * @FilePath     : /mxChat/include/Server/const.h
 * @Description  :  服务器端的const 定义
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 17:36:44
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <cstdint>
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>

namespace beast = boost::beast;  // from <boost/beast.hpp>
namespace http = beast::http;    // from <boost/beast/http.hpp>
namespace net = boost::asio;     // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;// from <boost/asio/ip/tcp.hpp>

constexpr short MAX_LENGTH = 1024 * 2;
//头部总长度
constexpr int_fast8_t HEAD_TOTAL_LEN = 4;
//头部id长度
constexpr int_fast8_t HEAD_ID_LEN = 2;
//头部数据长度
constexpr int_fast8_t HEAD_DATA_LEN = 2;
constexpr short MAX_RECVQUE = 10000;
constexpr short MAX_SENDQUE = 1000;
constexpr std::string ALLIPV4ADDRESS = "0.0.0.0:";

/**
 * @author       : caomengxuan666
 * @brief        : 错误码
**/
enum ErrorCodes {
    SUCCESSFUL = 0,
    Error_Json = 1001,  //Json解析错误
    RPCGetFailed = 1002,//RPC请求错误
    VarifyExpired = 1003,
    VarifyCodeErr = 1004,
    UserExist = 1005,
    PasswdErr = 1006,
    ERR_NETWORK = 1007,
    UidInvalid = 1008,
    TokenInvalid = 1009
};

/**
 * @author       : caomengxuan666
 * @brief        : 消息ID
**/
enum MSG_IDS {
	MSG_CHAT_LOGIN = 1005, //用户登陆
	MSG_CHAT_LOGIN_RSP = 1006, //用户登陆回包
	ID_SEARCH_USER_REQ = 1007, //用户搜索请求
	ID_SEARCH_USER_RSP = 1008, //搜索用户回包
	ID_ADD_FRIEND_REQ = 1009, //申请添加好友请求
	ID_ADD_FRIEND_RSP  = 1010, //申请添加好友回复
	ID_NOTIFY_ADD_FRIEND_REQ = 1011,  //通知用户添加好友申请
	ID_AUTH_FRIEND_REQ = 1013,  //认证好友请求
	ID_AUTH_FRIEND_RSP = 1014,  //认证好友回复
	ID_NOTIFY_AUTH_FRIEND_REQ = 1015, //通知用户认证好友申请
	ID_TEXT_CHAT_MSG_REQ = 1017, //文本聊天信息请求
	ID_TEXT_CHAT_MSG_RSP = 1018, //文本聊天信息回复
	ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, //通知用户文本聊天信息
};

constexpr std::string USERIPPREFIX= "uip_";
constexpr std::string USERTOKENPREFIX= "utoken_";
constexpr std::string IPCOUNTPREFIX= "ipcount_";
constexpr std::string USER_BASE_INFO = "ubaseinfo_";
constexpr std::string LOGIN_COUNT = "logincount";
constexpr std::string NAME_INFO = "nameinfo_";
