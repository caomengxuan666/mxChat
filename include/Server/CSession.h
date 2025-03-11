/**
 * @FilePath     : /mxChat/include/Server/CSession.h
 * @Description  :  会话对象，被CServer直接管理。每一个会话具体控制每一个来自客户端的连接
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 17:46:05
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

/**
 * @author       : caomengxuan666
 * @brief        : 会话对象类，被CServer直接管理。会话具体控制每一个来自客户端的连接
**/
class CSession : public std::enable_shared_from_this<CSession> {
public:
    /**
     * @author       : caomengxuan666
     * @brief        : Csession的构造函数，每次调用自动进行对_session_id的初始化
     * @param         {io_context} &io_context:
     * @param         {CServer} *server:
     * @return        {*}
    **/
    CSession(boost::asio::io_context &io_context, CServer *server);
    /**
     * @author       : caomengxuan666
     * @brief        : 关闭套接字，清理动态分配的资源
    **/    
    ~CSession();
    /**
     * @author       : caomengxuan666
     * @brief        : 返回这个会话的socket对象引用
     * @return        {tcp::socket &} 会话中存储的socket对象引用
    **/    
    tcp::socket &GetSocket();
    /**
     * @author       : caomengxuan666
     * @brief        : 获取当前的会话ID
     * @return        {std::string &} 会话的ID的字符串
    **/    
    std::string &GetSessionId();
    /**
     * @author       : caomengxuan666
     * @brief        : 设置当前会话的用户ID
     * @param         {int} uid:
     * @return        {*}
    **/    
    void SetUserId(int uid);
    /**
     * @author       : caomengxuan666
     * @brief        : 获取当前会话的用户ID
     * @return        {*}
    **/    
    int GetUserId();
    /**
     * @author       : caomengxuan666
     * @brief        : 获取这个对话产生时候生成的Uuid
     * @return        {std::string &}} 此会话的Uuid
    **/    
    std::string GetUuid();
    /**
     * @author       : caomengxuan666
     * @brief        : 启动会话，执行异步读TCP报文头部操作
     * @return        {*}
    **/    
    void Start();
    /**
     * @author       : caomengxuan666
     * @brief        : 向客户端发送msg消息，可以设置最大消息长度
     * @param         {char} *msg:
     * @param         {short} max_length:
     * @param         {short} msgid:
     * @return        {*}
    **/    
    void Send(char *msg, short max_length, short msgid);
    /**
     * @author       : caomengxuan666
     * @brief        : 向客户端发送msg消息
     * @param         {string} msg:
     * @param         {short} msgid:
     * @return        {*}
    **/    
    void Send(std::string msg, short msgid);
    /**
     * @author       : caomengxuan666
     * @brief        : 关闭服务器会话
     * @return        {*}
    **/    
    void Close();
    /**
     * @author       : caomengxuan666
     * @brief        : 返回当前会话的共享指针
     * @return        {std::shared_ptr<CSession>} 此会话的共享指针
    **/    
    std::shared_ptr<CSession> SharedSelf();
    /**
     * @author       : caomengxuan666
     * @brief        : 异步读取报文内容
     * @param         {int} length:
     * @return        {*}
    **/    
    void AsyncReadBody(int length);
    /**
     * @author       : caomengxuan666
     * @brief        : 异步读取报文头部
     * @param         {int} total_len:
     * @return        {*}
    **/    
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

/**
 * @author       : caomengxuan666
 * @brief        : 逻辑节点，用于存放逻辑处理所需要的会话和报文
**/
class LogicNode {
    friend class LogicSystem;

public:
    LogicNode(shared_ptr<CSession>, shared_ptr<RecvNode>);

private:
    shared_ptr<CSession> _session;  // 会话
    shared_ptr<RecvNode> _recvnode; // 报文
};
