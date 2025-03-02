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

class RecvNode : public MsgNode {
    friend class LogicSystem;
public:
    RecvNode(short max_len, short msg_id)
        : MsgNode(max_len), _msg_id(msg_id) {}
	
	~RecvNode() {
		std::cout << "destruct RecvNode" << endl;
	}

private:
    short _msg_id;
};

class SendNode : public MsgNode {
    friend class LogicSystem;
public:
    SendNode(const char* msg, short max_len, short msg_id)
        : MsgNode(max_len), _msg_id(msg_id) {
        // Initialize the message data if needed
        if (msg && strlen(msg) <= static_cast<size_t>(max_len)) {
            strncpy(_data, msg, max_len);
            _cur_len = static_cast<short>(strlen(msg));
        }
    }
	~SendNode(){
		std::cout << "destruct SendNode" << endl;
	}

private:
    short _msg_id;
};