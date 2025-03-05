/**
 * @FilePath     : /mxChat/include/Client/HttpMgr.h
 * @Description  : 用户通过Http与GateServer交互的类
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 21:39:28
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include "Singleton.h"
#include "global.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <QUrl>
#include <Server/const.h>
#include <memory>

/**
 * @author       : caomengxuan
 * @brief        : 用户通过Http与GateServer交互的类
**/
class HttpMgr : public QObject, public Singleton<HttpMgr>, public std::enable_shared_from_this<HttpMgr> {
    Q_OBJECT

public:
    ~HttpMgr();
    /**
     * @author       : caomengxuan
     * @brief        : 向GateServer发送Post请求体
     * @param         {QUrl} url:
     * @param         {QJsonObject} json:
     * @param         {ReqId} req_id:
     * @param         {Modules} mod:
     * @return        {*}
    **/    
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);

private:
    friend class Singleton<HttpMgr>;
    HttpMgr();
    QNetworkAccessManager _manager;
private slots:
    /**
     * @author       : caomengxuan
     * @brief        : http请求结束的槽函数
     * @param         {ReqId} id:
     * @param         {QString} res:
     * @param         {ErrorCodes} err:
     * @param         {Modules} mod:
     * @return        {*}
    **/
    void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);

signals:
    /**
     * @author       : caomengxuan
     * @brief        : http请求结束的信号
     * @param         {ReqId} id:
     * @param         {QString} res:
     * @param         {ErrorCodes} err:
     * @param         {Modules} mod:
     * @return        {*}
    **/
    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
    /**
     * @author       : caomengxuan
     * @brief        : 注册请求结束的信号
     * @param         {ReqId} id:
     * @param         {QString} res:
     * @param         {ErrorCodes} err:
     * @return        {*}
    **/    
    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    /**
     * @author       : caomengxuan
     * @brief        : 重置密码请求结束的信号
     * @param         {ReqId} id:
     * @param         {QString} res:
     * @param         {ErrorCodes} err:
     * @return        {*}
    **/    
    void sig_reset_mod_finish(ReqId id, QString res, ErrorCodes err);
    /**
     * @author       : caomengxuan
     * @brief        : 登录请求结束的信号
     * @param         {ReqId} id:
     * @param         {QString} res:
     * @param         {ErrorCodes} err:
     * @return        {*}
    **/    
    void sig_login_mod_finish(ReqId id, QString res, ErrorCodes err);
};