#pragma once

#include "Singleton.h"
#include <functional>
#include <map>
#include "const.h"
#include<queue>

class LogicNode;
class CSession;

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;
typedef  function<void(shared_ptr<CSession>, const short &msg_id, const string &msg_data)> FunCallBack;
class LogicSystem :public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem();
	void PostMsgToQue(shared_ptr <LogicNode> msg);
	void RegisterCallBacks();
	void DealMsg();
	void LoginHandler(shared_ptr<CSession> session, const short &msg_id, const string &msg_data);
private:
	LogicSystem();
	std::map<std::string, HttpHandler> _post_handlers;
	std::map<std::string, HttpHandler> _get_handlers;
	std::map<short, FunCallBack> _fun_callbacks;
	std::thread _worker_thread;
	std::queue<shared_ptr<LogicNode>> _msg_que;
	std::mutex _mutex;
	std::condition_variable _consume;
	bool b_stop;
};