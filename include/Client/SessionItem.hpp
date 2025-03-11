/**
 * @FilePath     : /mxChat/include/Client/SessionItem.hpp
 * @Description  :  主页面的会话界面
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-11 17:25:22
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include "Client/BubbleWidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <qcontainerfwd.h>
#include <qvariant.h>

/**
 * @author       : caomengxuan666
 * @brief        : 会话列表项
**/

/**
 * @struct       : 会话列表的数据存储
 * @author       : caomengxuan666
 * @brief        : 
 * @return        {*}
**/
struct SessionItemData {
    QString name;
    QString lastMessage;
    QString lastTime;
    int unreadCount;//todo
    QImage avatar;  //todo
};

/**
 * @author       : caomengxuan
 * @brief        : 会话具体的消息
 * @return        {*}
**/
struct SessionMsg {
    //消息的发送者
    QString sender;
    QString time;
    QString content;
    MessageType type;
};

/**
 * @author       : caomengxuan
 * @brief        : 会话聊天的数据
 * @return        {*}
**/
struct SessionChatData {
    //会话名称
    QString name;
    //存储消息
    QList<SessionMsg> messages;
};

class SessionItem : public QWidget {
    Q_OBJECT
public:
    explicit SessionItem(const SessionItemData &data, QWidget *parent = nullptr) noexcept
        : QWidget(parent), _sessionData(data) {
        setupUI(data);
    }

private:
    /**
     * @author       : caomengxuan666
     * @brief        : 
     * @param         {SessionItemData} &data:
     * @return        {*}
    **/
    void setupUI(const SessionItemData &data) {
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(12, 8, 12, 8);

        // 头像
        QLabel *avatar = new QLabel(this);
        avatar->setFixedSize(48, 48);
        avatar->setStyleSheet(R"(
            background: #00B4FF;
            border-radius: 24px;
            color: white;
            font-weight: bold;
        )");
        avatar->setText(data.name.left(1));
        avatar->setAlignment(Qt::AlignCenter);

        // 右侧信息
        QWidget *infoWidget = new QWidget(this);
        QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
        infoLayout->setContentsMargins(0, 0, 0, 0);

        // 名称和时间
        QWidget *topWidget = new QWidget(infoWidget);
        QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
        topLayout->setContentsMargins(0, 0, 0, 0);

        QLabel *nameLabel = new QLabel(data.name, topWidget);
        nameLabel->setStyleSheet("color: #DCDDDE; font-weight: 600;");

        QLabel *timeLabel = new QLabel(data.lastTime, topWidget);
        timeLabel->setStyleSheet("color: #7A7C80; font-size: 12px;");

        topLayout->addWidget(nameLabel);
        topLayout->addWidget(timeLabel);

        // 最后消息
        QLabel *msgLabel = new QLabel(data.lastMessage, infoWidget);
        msgLabel->setStyleSheet("color: #7A7C80; font-size: 13px;");
        msgLabel->setMaximumWidth(200);
        msgLabel->setWordWrap(true);

        infoLayout->addWidget(topWidget);
        infoLayout->addWidget(msgLabel);

        layout->addWidget(avatar);
        layout->addWidget(infoWidget, 1);
    }

    const SessionItemData &SessionData() const {
        return _sessionData;
    }

private:
    SessionItemData _sessionData;
};
