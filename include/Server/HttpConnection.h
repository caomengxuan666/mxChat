/**
 * @FilePath     : /mxChat/include/Server/HttpConnection.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-02 22:07:46
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once
#include "const.h"

class HttpConnection : public std::enable_shared_from_this<HttpConnection> {
    friend class LogicSystem;
    friend class GateLogic;

public:
    HttpConnection(tcp::socket socket);
    void Start();
    tcp::socket &GetSocket();

private:
    void CheckDeadline();
    void WriteResponse();
    void HandleReq();
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