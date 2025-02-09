/**
 * @FilePath     : /mxChat/src/ChatClient/MessageWidget.cpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-09 14:19:14
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
// MessageWidget.cpp
#include "Client/MessageWidget.h"

MessageWidget::MessageWidget(const QString &sender,
                             const QString &time,
                             const QString &content,
                             MessageType type,
                             QWidget *parent)
    : QWidget(parent) {
    setupUI(sender, time, content, type);
}

void MessageWidget::setupUI(const QString &sender, const QString &time, const QString &content, MessageType type) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *messageLayout = new QHBoxLayout();

    // 头像
    QLabel *avatar = new QLabel(this);
    avatar->setFixedSize(40, 40);
    avatar->setStyleSheet("background: #00B4FF; border-radius: 20px;");
    avatar->setAlignment(Qt::AlignCenter);

    // 消息气泡
    QLabel *messageBubble = new QLabel(content, this);
    messageBubble->setWordWrap(true);
    messageBubble->setStyleSheet("padding: 10px; border-radius: 12px;");

    // 时间戳
    QLabel *timestamp = new QLabel(time, this);
    timestamp->setStyleSheet("font-size: 12px; color: #999;");

    // 根据消息类型设置样式和布局
    if (type == MessageType::Self) {
        messageBubble->setStyleSheet("background: #00B4FF; color: white; border-radius: 12px 12px 0 12px;");
        messageLayout->addWidget(messageBubble);
        messageLayout->addWidget(avatar);
        messageLayout->setAlignment(Qt::AlignRight);
    } else {
        messageBubble->setStyleSheet("background: #FFFFFF; color: #333; border-radius: 12px 12px 12px 0; box-shadow: 0 2px 4px rgba(0,0,0,0.05);");
        messageLayout->addWidget(avatar);
        messageLayout->addWidget(messageBubble);
        messageLayout->setAlignment(Qt::AlignLeft);
    }

    // 添加到主布局
    mainLayout->addLayout(messageLayout);
    mainLayout->addWidget(timestamp, 0, Qt::AlignRight);
    mainLayout->setContentsMargins(12, 6, 12, 6);
}