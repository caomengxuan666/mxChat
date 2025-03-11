/**
 * @FilePath     : /mxChat/include/Client/TcpMgr.h
 * @Description  :  在客户端与服务端交互的类
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-04 22:38:51
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#ifndef TCPMGR_H
#define TCPMGR_H

#include "Singleton.h"
#include "global.h"
#include <QObject>
#include <QTcpSocket>

/**
 * @author       : caomengxuan666
 * @brief        : 客户端管理的服务器信息，有{Uid,主机，端口号，Token}
 * @return        {*}
**/
struct ServerInfo {
    int Uid;
    QString Host;
    QString Port;
    QString Token;
};


/**
 * @author       : caomengxuan666
 * @brief        : 客户端管理服务器的类，负责连接服务器，发送消息，接收消息，断开连接，重连等操作
**/
class TcpMgr : public QObject, public Singleton<TcpMgr>, public std::enable_shared_from_this<TcpMgr> {
    Q_OBJECT
public:
    TcpMgr();

private:
    /**
     * @author       : caomengxuan666
     * @brief        : 初始化处理函数
     * @return        {*}
    **/
    void initHandlers();
    /**
     * @author       : caomengxuan666
     * @brief        : 处理数据
     * @param         {ReqId} id:
     * @param         {int} len:
     * @param         {QByteArray} data:
     * @return        {*}
    **/
    void handleMsg(ReqId id, int len, QByteArray data);

    QMap<ReqId, std::function<void(ReqId id, int len, QByteArray data)>> _handlers;
public slots:
    /**
     * @author       : caomengxuan666
     * @brief        : 连接成功
     * @return        {*}
    **/
    void slot_tcp_connect(ServerInfo);
    /**
     * @author       : caomengxuan666
     * @brief        : 发送数据
     * @param         {ReqId} reqId:
     * @param         {QString} data:
     * @return        {*}
    **/    
    void slot_send_data(ReqId reqId, QString data);
signals:
    /**
     * @author       : caomengxuan666
     * @brief        : 连接成功
     * @param         {bool} bsuccess:
     * @return        {*}
    **/
    void sig_con_success(bool bsuccess);
    /**
     * @author       : caomengxuan666
     * @brief        : 发送信息
     * @param         {ReqId} reqId:
     * @param         {QString} data:
     * @return        {*}
    **/    
    void sig_send_data(ReqId reqId, QString data);
    /**
     * @author       : caomengxuan666
     * @brief        : 登录失败
     * @param         {int} err:
     * @return        {*}
    **/
    void sig_login_failed(int err);
    /**
     * @author       : caomengxuan666
     * @brief        : 切换聊天窗口
     * @return        {*}
    **/    
    void sig_swich_chatdlg();

private:
    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;
    bool _b_recv_pending;
    quint16 _message_id;
    quint16 _message_len;
};

#endif// TCPMGR_H