/**
 * @FilePath     : /mxChat/include/Client/TcpMgr.h
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-26 14:06:59
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#ifndef TCPMGR_H
#define TCPMGR_H

#include "Singleton.h"
#include "global.h"
#include <QObject>
#include <QTcpSocket>

struct ServerInfo{
    int Uid;
    QString Host;
    QString Port;
    QString Token;
};


class TcpMgr : public QObject, public Singleton<TcpMgr>, public std::enable_shared_from_this<TcpMgr> {
    Q_OBJECT
public:
    TcpMgr();

private:
    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;
    bool _b_recv_pending;
    quint16 _message_id;
    quint16 _message_len;
public slots:
    void slot_tcp_connect(ServerInfo);
    void slot_send_data(ReqId reqId, QString data);
signals:
    void sig_con_success(bool bsuccess);
    void sig_send_data(ReqId reqId, QString data);
};

#endif// TCPMGR_H