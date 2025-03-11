#pragma once

#include "Client/BubbleWidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <cstdint>
#include <qcontainerfwd.h>
#include <qvariant.h>
#include<QPropertyAnimation>
#include<QGraphicsOpacityEffect>

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
    void updateUnreadCount(int count) {
        _sessionData.unreadCount = count;
        if (unreadLabel) {
            count = std::min(count, 99);
            unreadLabel->setText(count > 0 ? QString::number(count) : "");
            unreadLabel->setVisible(count > 0);
            
            // 添加动画效果
            QPropertyAnimation *animation = new QPropertyAnimation(unreadLabel, "geometry");
            animation->setDuration(150);
            animation->setKeyValueAt(0, QRect(unreadLabel->x()+2, unreadLabel->y()+2, 16, 16));
            animation->setKeyValueAt(1, unreadLabel->geometry());
            animation->start();
        }
    }

    void updateLastMessage(const QString &msg, const QString &time) {
        _sessionData.lastMessage = msg;
        _sessionData.lastTime = time;
        
        // 限制消息长度
        QString elidedMsg = msgLabel->fontMetrics().elidedText(
            msg, Qt::ElideRight, msgLabel->width() - 20
        );
        
        msgLabel->setText(elidedMsg);
        timeLabel->setText(time);
        
        // 添加渐变动画
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(msgLabel);
        msgLabel->setGraphicsEffect(effect);
        
        QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity");
        animation->setDuration(500);
        animation->setStartValue(0);
        animation->setEndValue(1);
        animation->start();
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
    
        // 头像区域
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
    
        // 未读消息标签（提升为成员变量）
        unreadLabel = new QLabel(this);
        unreadLabel->setFixedSize(20, 20);
        unreadLabel->setStyleSheet(R"(
            background-color: red;
            border-radius: 10px;
            color: white;
            font-weight: bold;
            font-size: 12px;
            qproperty-alignment: 'AlignCenter';
            box-shadow: 2px 2px 4px rgba(0, 0, 0, 0.3);
        )");
        unreadLabel->move(avatar->width() - unreadLabel->width() + 10, 10);
        unreadLabel->setText(data.unreadCount > 0 ? QString::number(std::min(data.unreadCount, 99)) : "");
        unreadLabel->setVisible(data.unreadCount > 0);
    
        // 右侧信息区域
        QWidget *infoWidget = new QWidget(this);
        QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
        infoLayout->setContentsMargins(0, 0, 0, 0);
    
        // 顶部（名称 + 时间）
        QWidget *topWidget = new QWidget(infoWidget);
        QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
        topLayout->setContentsMargins(0, 0, 0, 0);
    
        QLabel *nameLabel = new QLabel(data.name, topWidget);
        nameLabel->setStyleSheet("color: #DCDDDE; font-weight: 600;");
    
        timeLabel = new QLabel(data.lastTime, topWidget); // 提升为成员变量
        timeLabel->setStyleSheet("color: #7A7C80; font-size: 12px;");
    
        topLayout->addWidget(nameLabel);
        topLayout->addWidget(timeLabel);
    
        // 最后消息
        msgLabel = new QLabel(data.lastMessage, infoWidget); // 提升为成员变量
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
    QLabel *unreadLabel;    
    QLabel *msgLabel;       
    QLabel *timeLabel;      
};
