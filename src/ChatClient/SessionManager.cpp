/**
 * @FilePath     : /mxChat/src/ChatClient/SessionManager.cpp
 * @Description  : 用于管理会话列表
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-11 19:52:10
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
    QListWidgetItem *item = new QListWidgetItem(_sessionListWidget);
    item->setSizeHint(QSize(280, 70));
    _sessionListWidget->addItem(item);

    SessionItem *sessionWidget = new SessionItem(data);
    _sessionListWidget->setItemWidget(item, sessionWidget);

    // 设置 Qt::UserRole 为会话名称
    item->setData(Qt::UserRole, data.name);
    _sessionList.append(item);
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
        for (const auto &msg : it.value().messages) {
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