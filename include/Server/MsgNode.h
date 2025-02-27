/**
 * @FilePath     : /mxChat/include/Server/MsgNode.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-26 18:06:30
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once
#include <string>
#include "const.h"
#include <iostream>
#include <boost/asio.hpp>
using namespace std;
using boost::asio::ip::tcp;
class LogicSystem;
class MsgNode
{
public:
        MsgNode(short max_len) :_total_len(max_len), _cur_len(0) {
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
        char* _data;
};

class RecvNode :public MsgNode {
        friend class LogicSystem;
public:
        RecvNode(short max_len, short msg_id);
private:
        short _msg_id;
};

class SendNode:public MsgNode {
        friend class LogicSystem;
public:
        SendNode(const char* msg,short max_len, short msg_id);
private:
        short _msg_id;
};
