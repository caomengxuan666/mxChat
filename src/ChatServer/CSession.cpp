/**
 * @FilePath     : /mxChat/src/ChatServer/CSession.cpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-04 16:31:43
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include <Server/CServer.h>
#include <Server/CSession.h>
#include <Server/LogicSystem.h>
#include <Server/const.h>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <spdlog/spdlog.h>


CSession::CSession(boost::asio::io_context &io_context, CServer *server) : _socket(io_context), _server(server), _b_close(false), _b_head_parse(false), _user_uid(0) {
    boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
    _session_id = boost::uuids::to_string(a_uuid);
    _recv_head_node = make_shared<MsgNode>(HEAD_TOTAL_LEN);
}


CSession::~CSession() {
    Close();// 确保套接字已关闭

    // 清理动态分配的资源
    if (_recv_head_node) {
        //_recv_head_node.reset();
    }
    if (_recv_msg_node) {
        //_recv_msg_node.reset();
    }
}

void CSession::HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self) {
	//增加异常处理
	try {
		if (!error) {
			std::lock_guard<std::mutex> lock(_send_lock);
			//cout << "send data " << _send_que.front()->_data+HEAD_LENGTH << endl;
			_send_que.pop();
			if (!_send_que.empty()) {
				auto& msgnode = _send_que.front();
				boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
					std::bind(&CSession::HandleWrite, this, std::placeholders::_1, shared_self));
			}
		}
		else {
			std::cout << "handle write failed, error is " << error.what() << endl;
			Close();
			_server->ClearSession(_session_id);
		}
	}
	catch (std::exception& e) {
		std::cerr << "Exception code : " << e.what() << endl;
	}
	
}

std::shared_ptr<CSession>CSession::SharedSelf() {
	return shared_from_this();
}

void CSession::Start() {
    AsyncReadHead(HEAD_TOTAL_LEN);
}

void CSession::AsyncReadHead(int total_len) {
    auto self = shared_from_this();
    AsyncReadFull(HEAD_TOTAL_LEN, [self, this](const boost::system::error_code &ec, std::size_t bytes_transfered) {
        try {
            if (ec) {
                spdlog::error("handle read failed, error is {}", ec.what());
                Close();
                _server->ClearSession(_session_id);
                return;
            }

            if (bytes_transfered < HEAD_TOTAL_LEN) {
                spdlog::info("read length not match, read [{}] , total [{}]", bytes_transfered, HEAD_TOTAL_LEN);
                Close();
                _server->ClearSession(_session_id);
                return;
            }

            _recv_head_node->Clear();
            memcpy(_recv_head_node->_data, _data, bytes_transfered);

            //获取头部MSGID数据
            short msg_id = 0;
            memcpy(&msg_id, _recv_head_node->_data, HEAD_ID_LEN);
            //网络字节序转化为本地字节序
            msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
            spdlog::info("msg_id is {}", msg_id);
            //id非法
            if (msg_id > MAX_LENGTH) {
                spdlog::info("invalid msg_id is {}", msg_id);
                _server->ClearSession(_session_id);
                return;
            }
            short msg_len = 0;
            memcpy(&msg_len, _recv_head_node->_data + HEAD_ID_LEN, HEAD_DATA_LEN);
            //网络字节序转化为本地字节序
            msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
            spdlog::info("msg_len is {}", msg_len);

            //id非法
            if (msg_len > MAX_LENGTH) {
                spdlog::info("invalid data length is {}", msg_len);
                _server->ClearSession(_session_id);
                return;
            }

            _recv_msg_node = make_shared<RecvNode>(msg_len, msg_id);
            AsyncReadBody(msg_len);
        } catch (std::exception &e) {
            spdlog::info("Exception code is {}", e.what());
        }
    });
}

//读取完整长度
void CSession::AsyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code &, std::size_t)> handler) {
    ::memset(_data, 0, MAX_LENGTH);
    AsyncReadLen(0, maxLength, handler);
}


//读取指定字节数
void CSession::AsyncReadLen(std::size_t read_len, std::size_t total_len,
                            std::function<void(const boost::system::error_code &, std::size_t)> handler) {
    auto self = shared_from_this();
    _socket.async_read_some(boost::asio::buffer(_data + read_len, total_len - read_len),
                            [read_len, total_len, handler, self](const boost::system::error_code &ec, std::size_t bytesTransfered) {
                                try {
                                    if (ec) {
                                        if (ec == boost::asio::error::eof) {
                                            spdlog::info("[CSession] Client disconnected, closing session.");
                                        } else {
                                            spdlog::error("[CSession] Read failed, error is {}", ec.what());
                                        }
                                        handler(ec, read_len + bytesTransfered);
                                        return;
                                    }

                                    if (read_len + bytesTransfered >= total_len) {
                                        handler(ec, read_len + bytesTransfered);
                                        return;
                                    }

                                    self->AsyncReadLen(read_len + bytesTransfered, total_len, handler);
                                } catch (const std::exception &e) {
                                    spdlog::error("[CSession] Exception in async_read_some: {}", e.what());
                                    handler(boost::asio::error::operation_aborted, read_len + bytesTransfered);
                                }
                            });
}

void CSession::AsyncReadBody(int total_len) {
    auto self = shared_from_this();
    AsyncReadFull(total_len, [self, this, total_len](const boost::system::error_code &ec, std::size_t bytes_transfered) {
        try {
            if (ec) {
                spdlog::info("handle read failed, error is {}", ec.what());
                Close();
                _server->ClearSession(_session_id);
                return;
            }

            if (bytes_transfered < total_len) {
                spdlog::info("read length not match, read [{}] , total [{}]", bytes_transfered, total_len);
                Close();
                _server->ClearSession(_session_id);
                return;
            }

            memcpy(_recv_msg_node->_data, _data, bytes_transfered);
            _recv_msg_node->_cur_len += bytes_transfered;
            _recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
            spdlog::info("receive data is {}", _recv_msg_node->_data);
            //此处将消息投递到逻辑队列中
            LogicSystem::GetInstance()->PostMsgToQue(make_shared<LogicNode>(shared_from_this(), _recv_msg_node));
            //继续监听头部接受事件
            AsyncReadHead(HEAD_TOTAL_LEN);
        } catch (std::exception &e) {
            spdlog::info("Exception code is {}", e.what());
        }
    });
}

void CSession::Send(std::string msg, short msgid) {
	std::lock_guard<std::mutex> lock(_send_lock);
	int send_que_size = _send_que.size();
	if (send_que_size > MAX_SENDQUE) {
		std::cout << "session: " << _session_id << " send que fulled, size is " << MAX_SENDQUE << endl;
		return;
	}

	_send_que.push(make_shared<SendNode>(msg.c_str(), msg.length(), msgid));
	if (send_que_size > 0) {
		return;
	}
	auto& msgnode = _send_que.front();
	boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
		std::bind(&CSession::HandleWrite, this, std::placeholders::_1, SharedSelf()));
}


void CSession::Send(char* msg, short max_length, short msgid) {
	std::lock_guard<std::mutex> lock(_send_lock);
	int send_que_size = _send_que.size();
	if (send_que_size > MAX_SENDQUE) {
		std::cout << "session: " << _session_id << " send que fulled, size is " << MAX_SENDQUE << endl;
		return;
	}

	_send_que.push(make_shared<SendNode>(msg, max_length, msgid));
	if (send_que_size>0) {
		return;
	}
	auto& msgnode = _send_que.front();
	boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len), 
		std::bind(&CSession::HandleWrite, this, std::placeholders::_1, SharedSelf()));
}

tcp::socket &CSession::GetSocket() {
    return _socket;
}

std::string &CSession::GetSessionId() {
    return _session_id;
}
void CSession::SetUserId(int uid) {
}

int CSession::GetUserId() {
    return 0;
}

std::string CSession::GetUuid() {
    return _session_id;
}

void CSession::Close() {
    if (!_b_close) {
        _socket.close();
        _b_close = true;
        spdlog::info("[CSession] Session {} closed.", _session_id);
    }
}

LogicNode::LogicNode(shared_ptr<CSession> session,
                     shared_ptr<RecvNode> recvnode) : _session(session), _recvnode(recvnode) {
}