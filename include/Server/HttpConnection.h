/**
 * @FilePath     : /mxChat/include/Server/HttpConnection.h
 * @Description  :  HttpConnection类，处理来自客户端、浏览器的Http请求
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 18:09:53
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once
#include "const.h"

class HttpConnection : public std::enable_shared_from_this<HttpConnection> {
    friend class LogicSystem;
    friend class GateLogic;

public:
    /**
     * @author       : caomengxuan666
     * @brief        : 构造函数,通过move将socket所有权转移到HttpConnetcion类
     * @param         {socket} socket:
     * @return        {*}
    **/
    HttpConnection(tcp::socket socket);
    /**
     * @author       : caomengxuan666
     * @brief        : 开始异步进行读操作，并通过别的函数处理读到的数据
     * @return        {*}
    **/   
    void Start();
    tcp::socket &GetSocket();

private:
    /**
     * @author       : caomengxuan666
     * @brief        : 启动一个计时器检查超时
     * @return        {*}
    **/
    void CheckDeadline();
    /**
     * @author       : caomengxuan666
     * @brief        : 异步写入响应，并将计时器重置。
     * @return        {*}
    **/    
    void WriteResponse();
    /**
     * @author       : caomengxuan666
     * @brief        : 根据请求选择对应的Http标准，设置Http为短链接,转发给gatelogic处理get和post请求
     * @return        {*}
    **/    
    void HandleReq();
    /**
     * @author       : caomengxuan666
     * @brief        : // 解析GET请求参数
     * @return        {*}
    **/    
    void PreParseGetParam();

    tcp::socket _socket;
    // The buffer for performing reads.
    beast::flat_buffer _buffer{8192};

    // The request message.
    http::request<http::dynamic_body> _request;

    // The response message.
    http::response<http::dynamic_body> _response;

    // The timer for putting a deadline on connection processing.
    net::steady_timer deadline_{
            _socket.get_executor(), std::chrono::seconds(60)};
    std::string _get_url;
    std::unordered_map<std::string, std::string> _get_params;
};