/**
 * @FilePath     : /mxChat/include/Client/MainWidget.h
 * @Description  :  主要的聊天界面
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-04 22:22:18
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
// MainWidget.h
#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "Client/BubbleWidget.h"
#include <Client/Login.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPainter>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QTextBrowser>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <qtextedit.h>
#include <qtmetamacros.h>

class MainWidget;

class MainWidget : public QWidget {
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

public slots:
    void onLoginSuccess();

private:
    /**
    * @author       : caomengxuan
    * @brief        : 创建所有的动态控件,导航栏，动态列表，输入区域，气泡区域
    * @return        {*}
    **/
    void setupUI();
    /**
     * @author       : caomengxuan
     * @brief        : 管理MainWidget的样式
     * @return        {*}
    **/    
    void setupStyle();
    /**
     * @author       : caomengxuan
     * @brief        : 侧边会话栏项目
     * @param         {QString} &name:会话的姓名
     * @param         {QString} &msg: 发送的信息
     * @param         {QString} &time:会话信息的时间
     * @return        {*}
    **/    
    void addSessionItem(const QString &name, const QString &msg, const QString &time);
    /**
     * @author       : caomengxuan
     * @brief        : 
     * @param         {QString} &sender:
     * @param         {QString} &time:
     * @param         {QString} &content:
     * @param         {MessageType} type:
     * @return        {*}
    **/    
    void addMessasge(const QString &sender, const QString &time, const QString &content, MessageType type);

    /**
    * @author       : caomengxuan
    * @brief        : 用于创建侧边导航栏
    * @param         {QString} &iconName:
    * @param         {bool} isAvatar:
    * @return        {QToolButton} 侧边导航栏的每一个控件
    **/
    QToolButton *createNavButton(const QString &iconName, bool isAvatar = false);

    /**
     * @author       : caomengxuan
     * @brief        : 具体会话点击触发切换到该会话的回调函数
     * @param         {QListWidgetItem} *item:
     * @return        {*}
    **/    
    void onSessionItemClicked(QListWidgetItem *item);
    /**
     * @author       : caomengxuan
     * @brief        : 用于刷新聊天区域
     * @param         {QString} &sessionName:
     * @return        {*}
    **/    
    void updateChatArea(const QString &sessionName);
    [[maybe_unused]]
    void addTimeDivider(const QString &timeText);
    // 组件
    QListWidget *sessionList;
    QScrollArea *chatArea;
    QTextEdit *messageInput;
    QPushButton *sendButton;
    QWidget *inputPanel;
    QVBoxLayout *chatAreaLayout;
};


#endif// MAINWIDGET_H