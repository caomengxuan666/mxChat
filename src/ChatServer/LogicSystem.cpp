/**
 * @FilePath     : /mxChat/src/ChatServer/LogicSystem.cpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-02 22:40:09
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include "Server/LogicSystem.h"
#include "DataBase/MysqlMgr.h"
#include "DataBase/RedisMgr.h"
#include "Server/HttpConnection.h"
#include "Server/StatusGrpcClient.hpp"
#include "Server/VarifyGrpcClient.h"
#include <Server/CSession.h>
#include <spdlog/spdlog.h>


LogicSystem::~LogicSystem() {
}


void LogicSystem::PostMsgToQue(shared_ptr<LogicNode> msg) {
    std::unique_lock<std::mutex> unique_lk(_mutex);
    _msg_que.push(msg);
    //由0变为1则发送通知信号
    if (_msg_que.size() == 1) {
        unique_lk.unlock();
        _consume.notify_one();
    }
}
LogicSystem::LogicSystem() : b_stop(false) {
    RegisterCallBacks();
    _worker_thread=std::thread(&LogicSystem::DealMsg,this);
}


void HttpConnection::WriteResponse() {
    auto self = shared_from_this();
    _response.content_length(_response.body().size());
    http::async_write(
            _socket,
            _response,
            [self](beast::error_code ec, std::size_t) {
                self->_socket.shutdown(tcp::socket::shutdown_send, ec);
                self->deadline_.cancel();
            });
}

void HttpConnection::CheckDeadline() {
    auto self = shared_from_this();

    deadline_.async_wait(
            [self](beast::error_code ec) {
                if (!ec) {
                    // Close socket to cancel any outstanding operation.
                    self->_socket.close(ec);
                }
            });
}

void LogicSystem::RegisterCallBacks() {
    _fun_callbacks[MSG_CHAT_LOGIN] = std::bind(&LogicSystem::LoginHandler, this,
                                               placeholders::_1, placeholders::_2, placeholders::_3);
}

void LogicSystem::LoginHandler(shared_ptr<CSession> session, const short &msg_id, const string &msg_data) {
    Json::Reader reader;
    Json::Value root;
    reader.parse(msg_data, root);
    auto uid = root["uid"].asInt();
    std::cout << "user login uid is  " << uid << " user token  is "
              << root["token"].asString() << endl;

    // 从状态服务器获取token匹配是否准确
    auto rsp = StatusGrpcClient::GetInstance()->Login(uid, root["token"].asString());
    Json::Value rtvalue;

    // 使用 std::unique_ptr 和 std::function 实现 defer
    auto defer = std::unique_ptr<void, std::function<void(void *)>>(nullptr, [&rtvalue, session](void *) {
        std::string return_str = rtvalue.toStyledString();
        session->Send(return_str, MSG_CHAT_LOGIN_RSP);
    });

    rtvalue["error"] = rsp.error();
    if (rsp.error() != ErrorCodes::SUCCESSFUL) {
        return;
    }

    // 内存中查询用户信息
    //todo
    /*
    auto find_iter = _users.find(uid);
    std::shared_ptr<UserInfo> user_info = nullptr;
    if (find_iter == _users.end()) {
        // 查询数据库
        user_info = MysqlMgr::GetInstance()->GetUser(uid);
        if (user_info == nullptr) {
            rtvalue["error"] = ErrorCodes::UidInvalid;
            return;
        }

        _users[uid] = user_info;
    } else {
        user_info = find_iter->second;
    }
*/
    
    rtvalue["uid"] = uid;
    rtvalue["token"] = rsp.token();
    //rtvalue["name"] = user_info->name;
}

void LogicSystem::DealMsg() {
	for (;;) {
		std::unique_lock<std::mutex> unique_lk(_mutex);
		//判断队列为空则用条件变量阻塞等待，并释放锁
		while (_msg_que.empty() && !b_stop) {
			_consume.wait(unique_lk);
		}

		//判断是否为关闭状态，把所有逻辑执行完后则退出循环
		if (b_stop ) {
			while (!_msg_que.empty()) {
				auto msg_node = _msg_que.front();
				cout << "recv_msg id  is " << msg_node->_recvnode->_msg_id << endl;
				auto call_back_iter = _fun_callbacks.find(msg_node->_recvnode->_msg_id);
				if (call_back_iter == _fun_callbacks.end()) {
					_msg_que.pop();
					continue;
				}
				call_back_iter->second(msg_node->_session, msg_node->_recvnode->_msg_id,
					std::string(msg_node->_recvnode->_data, msg_node->_recvnode->_cur_len));
				_msg_que.pop();
			}
			break;
		}

		//如果没有停服，且说明队列中有数据
		auto msg_node = _msg_que.front();
		cout << "recv_msg id  is " << msg_node->_recvnode->_msg_id << endl;
		auto call_back_iter = _fun_callbacks.find(msg_node->_recvnode->_msg_id);
		if (call_back_iter == _fun_callbacks.end()) {
			_msg_que.pop();
			std::cout << "msg id [" << msg_node->_recvnode->_msg_id << "] handler not found" << std::endl;
			continue;
		}
		call_back_iter->second(msg_node->_session, msg_node->_recvnode->_msg_id, 
			std::string(msg_node->_recvnode->_data, msg_node->_recvnode->_cur_len));
		_msg_que.pop();
	}
}