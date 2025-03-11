/**
 * @FilePath     : /mxChat/include/Client/SessionManager.h
 * @Description  : 用于管理会话列表
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-11 17:17:08
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include "SessionItem.hpp"
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QString>

/**
 * @author       : caomengxuan
 * @brief        : 客户端会话列表的管理类
 * @return        {*}
**/
class SessionManager {
public:
    SessionManager(QListWidget *sessionListWidget);

    /**
     * @author       : caomengxuan
     * @brief        : 清空客户端会话列表
     * @return        {*}
    **/
    void addSessionItem(const SessionItemData &data);
    /**
     * @author       : caomengxuan
     * @brief        : 清空客户端会话列表
     * @return        {*}
    **/    
    void clearSessionItems();
    /**
     * @author       : caomengxuan
     * @brief        : 移除客户端会话列表
     * @param         {QString} &name:
     * @return        {*}
    **/
    void removeSessionItem(const QString &name);
    /**
     * @author       : caomengxuan
     * @brief        : 根据名称搜索并且返回具体的单个列表项
     * @param         {QString} &name:
     * @return        {QListWidgetItem *} 具体的列表项
    **/
    QListWidgetItem *findSessionItem(const QString &name);
    /**
     * @author       : caomengxuan
     * @brief        : 获取会话列表的Widget
     * @return        {QListWidget *} 会话列表的Widget
    **/
    QListWidget *getSessionListWidget() const;

    /**
     * @author       : caomengxuan
     * @brief        : 获取某个会话的聊天记录
     * @param         {QString} &name:
     * @return        {QList<Message>} 聊天记录列表
    **/
    void addSessionData(const SessionChatData &chatdata);
    /**
     * @author       : caomengxuan
     * @brief        : 获取某个会话的聊天记录
     * @param         {QString} &name:
     * @return        {SessionChatData} 返回的聊天记录内容
    **/    
    SessionChatData getMessagesForSession(const QString &session_name);

private:
    QListWidget *_sessionListWidget;
    QList<QListWidgetItem *> _sessionList;
    QMap<QString, SessionChatData> _sessionData;
};