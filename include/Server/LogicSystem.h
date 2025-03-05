/**
 * @FilePath     : /mxChat/include/Server/LogicSystem.h
 * @Description  :  Chatserver的业务逻辑处理
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 18:19:29
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include "Singleton.h"
#include "const.h"
#include <functional>
#include <map>
#include <queue>

class LogicNode;
class CSession;


typedef std::function<void(std::shared_ptr<CSession>, const short &msg_id, const std::string &msg_data)> FunCallBack;
/**
 * @author       : caomengxuan
 * @brief        : Chatserver的业务逻辑处理类
**/
class LogicSystem : public Singleton<LogicSystem> {
    friend class Singleton<LogicSystem>;

public:
    ~LogicSystem();
    /**
	* @author       : caomengxuan
	* @brief        : 将消息push到队列
	* @param         {shared_ptr <LogicNode>} msg:
	* @return        {*}
	**/
    void PostMsgToQue(std::shared_ptr<LogicNode> msg);
    /**
	* @author       : caomengxuan
	* @brief        : 用std::bind来注册不同消息处理的回调函数,处理消息队列的时候,不同的消息直接触发不同的函数
	* @return        {*}
	**/
    void RegisterCallBacks();
    /**
     * @author       : caomengxuan
     * @brief        : 解析Msg,通过Msg来触发对应先前注册的回调
     * @return        {*}
    **/	
    void DealMsg();
    /**
     * @author       : caomengxuan
     * @brief        : 登录的处理函数,从状态服务器获取token,并且在内存中查询核对用户信息,完成之后通过CSession->Send返回状态json
     * @param         {shared_ptr<CSession>} session:
     * @param         {short} &msg_id:
     * @param         {string} &msg_data:
     * @return        {*}
    **/	
    void LoginHandler(std::shared_ptr<CSession> session, const short &msg_id, const std::string &msg_data);

private:
    LogicSystem();

    std::map<short, FunCallBack> _fun_callbacks;
    std::thread _worker_thread;
    std::queue<std::shared_ptr<LogicNode>> _msg_que;
    std::mutex _mutex;
    std::condition_variable _consume;
    bool b_stop;
};