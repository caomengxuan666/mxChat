/**
 * @FilePath     : /mxChat/src/ChatClient/SessionManager.cpp
 * @Description  : 用于管理会话列表
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-11 23:22:40
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include "Client/SessionManager.h"
#include "Client/SessionItem.hpp"
#include <QGraphicsOpacityEffect>
#include <QListWidgetItem>
#include <QPropertyAnimation>
#include <QTimer>
#include <qforeach.h>
#include <qlistwidget.h>

SessionManager::SessionManager(QListWidget *sessionListWidget)
    : _sessionListWidget(sessionListWidget) {
}

void SessionManager::addSessionItem(const SessionItemData &data) {
    // 先检查是否已存在同名会话项，存在则先移除
    if (_sessionItem.contains(data.name)) {
        removeSessionItem(data.name);
    }

    QListWidgetItem *item = new QListWidgetItem(_sessionListWidget);
    item->setSizeHint(QSize(280, 70));
    _sessionListWidget->addItem(item);

    SessionItem *sessionWidget = new SessionItem(data);
    _sessionListWidget->setItemWidget(item, sessionWidget);

    // 存储会话项到映射中
    item->setData(Qt::UserRole, data.name);
    _sessionList.append(item);
    _sessionItem.insert(data.name, sessionWidget);// 新增插入映射操作
}


void SessionManager::clearSessionItems() {
    _sessionListWidget->clear();
    _sessionList.clear();
}

void SessionManager::removeSessionItem(const QString &name) {
    for (QListWidgetItem *item: _sessionList) {
        if (item->data(Qt::UserRole).toString() == name) {
            _sessionListWidget->removeItemWidget(item);
            delete item;
            _sessionList.removeOne(item);
            _sessionItem.remove(name);// 新增从映射中移除操作
            break;
        }
    }
}

QListWidgetItem *SessionManager::findSessionItem(const QString &name) {
    for (QListWidgetItem *item: _sessionList) {
        if (item->data(Qt::UserRole).toString() == name) {
            return item;
        }
    }
    return nullptr;
}

QListWidget *SessionManager::getSessionListWidget() const {
    return _sessionListWidget;
}

void SessionManager::addSessionData(const SessionChatData &chatdata) {
    // 如果已经存在该会话，则更新消息列表
    if (_sessionData.contains(chatdata.name)) {
        // 如果会话消息完全与之前的相同，则不更新

        _sessionData[chatdata.name].messages.append(chatdata.messages);
    } else {
        // 否则插入新的会话数据
        _sessionData[chatdata.name] = chatdata;
    }
}

SessionChatData SessionManager::getMessagesForSession(const QString &session_name) {
    qDebug() << "Searching for session with name:" << session_name;
    qDebug() << "Total sessions in _sessionData:" << _sessionData.size();

    for (auto it = _sessionData.begin(); it != _sessionData.end(); ++it) {
        qDebug() << "Session name:" << it.key();
        qDebug() << "Number of messages:" << it.value().messages.size();
        //输出具体的消息
        for (const auto &msg: it.value().messages) {
            qDebug() << "Message content:" << msg.content;
            qDebug() << "Message sender:" << msg.sender;
            qDebug() << "Message time:" << msg.time;
        }
    }

    // 检查是否存在该会话
    if (_sessionData.contains(session_name)) {
        qDebug() << "Session found. Returning messages.";
        return _sessionData[session_name];
    } else {
        qDebug() << "Session not found.";
        return SessionChatData();// 返回一个空的 SessionChatData
    }
}

// SessionManager.cpp 补充实现
void SessionManager::clearUnreadedCount(const QString &session_name) {
    if (_sessionItem.contains(session_name)) {
        // 获取对应的会话项控件
        SessionItem *itemWidget = _sessionItem.value(session_name);
        // 调用控件更新方法
        itemWidget->updateUnreadCount(0);

        // todo：同步更新本地数据存储
        if (_sessionData.contains(session_name)) {
            // 如果需要保持数据同步，可以在这里更新数据层
            // 注意：SessionChatData 目前没有 unreadCount 字段
            // 可能需要扩展数据结构
        }
    }
}

void SessionManager::updateSessionLastMessage(const QString &session_name, const QString &message, const QString &time) {
    if (_sessionItem.contains(session_name)) {
        SessionItem *itemWidget = _sessionItem.value(session_name);
        itemWidget->updateLastMessage(message, time);
    }
}