/**
 * @FilePath     : /mxChat/include/Server/CSession.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-04 16:32:03
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include <Server/MsgNode.h>
#include <Server/const.h>
#include <boost/system.hpp>
#include <functional>
#include <memory>
#include<queue>

class CServer;
class CSession : public std::enable_shared_from_this<CSession> {
public:
    CSession(boost::asio::io_context &io_context, CServer *server);
    ~CSession();
    tcp::socket &GetSocket();
    std::string &GetSessionId();
    void SetUserId(int uid);
    int GetUserId();
    std::string GetUuid();
    void Start();
    void Send(char *msg, short max_length, short msgid);
    void Send(std::string msg, short msgid);
    void Close();
    std::shared_ptr<CSession> SharedSelf();
    void AsyncReadBody(int length);
    void AsyncReadHead(int total_len);

private:
    void AsyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code &, std::size_t)> handler);
    void AsyncReadLen(std::size_t read_len, std::size_t total_len,
                      std::function<void(const boost::system::error_code &, std::size_t)> handler);


    void HandleWrite(const boost::system::error_code &error, std::shared_ptr<CSession> shared_self);
    tcp::socket _socket;
    std::string _session_id;
    char _data[MAX_LENGTH];
    CServer *_server;
    bool _b_close;
    std::mutex _send_lock;
    //收到的消息结构
    std::shared_ptr<RecvNode> _recv_msg_node;
    bool _b_head_parse;
    //收到的头部结构
    std::shared_ptr<MsgNode> _recv_head_node;
    std::queue<shared_ptr<SendNode> > _send_que;
    int _user_uid;
};

class LogicNode {
    friend class LogicSystem;

public:
    LogicNode(shared_ptr<CSession>, shared_ptr<RecvNode>);

private:
    shared_ptr<CSession> _session;
    shared_ptr<RecvNode> _recvnode;
};
