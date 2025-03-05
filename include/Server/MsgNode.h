/**
 * @FilePath     : /mxChat/include/Server/MsgNode.h
 * @Description  :  消息节点
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 18:24:45
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once
#include "const.h"
#include <boost/asio.hpp>
#include <iostream>
#include <string>
using namespace std;
using boost::asio::ip::tcp;
class LogicSystem;
/**
 * @author       : caomengxuan
 * @brief        : 消息节点,包含[当前接受长度,总长度,数据]
 * @note         : 消息节点作为基类，派生出两个子类，一个用于接受消息，一个用于发送消息
**/
class MsgNode {
public:
    /**
	* @author       : caomengxuan
	* @brief        : 构造函数
	* @param         {short} max_len: 最大长度
	* @note 		: 所有的派生类也都遵循开始的时候创建一个空的数组，并设置数组最后一个字符为'\0',构造时当前长度都为1
	* @return        {*}
	**/
    MsgNode(short max_len) : _total_len(max_len), _cur_len(0) {
        _data = new char[_total_len + 1]();
        _data[_total_len] = '\0';
    }

    ~MsgNode() {
        std::cout << "destruct MsgNode" << endl;
        delete[] _data;
    }

    void Clear() {
        ::memset(_data, 0, _total_len);
        _cur_len = 0;
    }

    short _cur_len;
    short _total_len;
    char *_data;
};

/**
 * @author       : caomengxuan
 * @brief        : 接收消息节点
**/
class RecvNode : public MsgNode {
    friend class LogicSystem;

public:
    /**
     * @author       : caomengxuan
     * @brief        : 构造函数
     * @param         {short} max_len: 最大消息长度
     * @param         {short} msg_id:  消息的id
     * @return        {*}
    **/
    RecvNode(short max_len, short msg_id)
        : MsgNode(max_len), _msg_id(msg_id) {}

    ~RecvNode() {
        std::cout << "destruct RecvNode" << endl;
    }

private:
    short _msg_id;
};

/**
 * @author       : caomengxuan
 * @brief        : 发送消息节点
**/
class SendNode : public MsgNode {
    friend class LogicSystem;

public:
    /**
     * @author       : caomengxuan
     * @brief        : 构造函数
     * @param         {char*} msg: 消息数据指针
     * @param         {short} max_len: 消息最大长度
     * @param         {short} msg_id: 消息id
     * @return        {*}
    **/
    SendNode(const char *msg, short max_len, short msg_id)
        : MsgNode(max_len), _msg_id(msg_id) {
        // Initialize the message data if needed
        if (msg && strlen(msg) <= static_cast<size_t>(max_len)) {
            strncpy(_data, msg, max_len);
            _cur_len = static_cast<short>(strlen(msg));
        }
    }
    ~SendNode() {
        std::cout << "destruct SendNode" << endl;
    }

private:
    short _msg_id;
};