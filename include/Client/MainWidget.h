/**
 * @FilePath     : /mxChat/include/Client/MainWidget.h
 * @Description  :  主要的聊天界面
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-14 20:04:24
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
// MainWidget.h
#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "Client/BubbleWidget.h"
#include "Client/SessionItem.hpp"
#include <Client/Login.h>
#include <Client/SessionManager.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QPainter>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QTextBrowser>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <qcontainerfwd.h>
#include <qlineedit.h>
#include <qmainwindow.h>
#include <qtextedit.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class MainWidget : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWidget(QMainWindow *parent = nullptr);
    ~MainWidget();

public slots:
    void onLoginSuccess();
private slots:
    void querySearchBar();
    void showChatPage();
    void showContactsPage();
    void showFilesPage();
    void showSettingsPage();
    void showUserHomePage();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void showScrollBars(bool show);

private:
    /**
    * @author       : caomengxuan666
    * @brief        : 创建所有的动态控件,导航栏，动态列表，输入区域，气泡区域
    * @return        {*}
    **/
    void setupUI();
    /**
     * @author       : caomengxuan666
     * @brief        : 管理MainWidget的样式
     * @return        {*}
    **/
    void setupStyle();
    /**
     * @author       : caomengxuan666
     * @brief        : 侧边会话栏项目
     * @param         {QString} &name:会话的姓名
     * @param         {QString} &msg: 发送的信息
     * @param         {QString} &time:会话信息的时间
     * @return        {*}
    **/
    void addSessionItem(const SessionItemData &data);
    /**
     * @author       : caomengxuan666
     * @brief        : 用于发送消息
     * @param         {QString} &session_name:会话名称
     * @param         {QString} &sender:
     * @param         {QString} &time:
     * @param         {QString} &content:
     * @param         {MessageType} type:
     * @return        {*}
    **/
    void addMessage(const QString &session_name, const QString &sender, const QString &time, const QString &content, MessageType type);


    /**
     * @author       : caomengxuan666
     * @brief        : 具体会话点击触发切换到该会话的回调函数
     * @param         {QListWidgetItem} *item:
     * @return        {*}
    **/
    void onSessionItemClicked(QListWidgetItem *item);
    /**
     * @author       : caomengxuan666
     * @brief        : 用于刷新聊天区域
     * @param         {QString} &sessionName:
     * @return        {*}
    **/
    void updateChatArea(const QString &sessionName);


    /**
     * @author       : caomengxuan666
     * @brief        : 获取当前会话的名称
     * @return        {QString} 当前会话的名称
    **/
    const QString getCurrentSessionName() const;

    void insertTestCase();

    void startTestMessagePush();

    // 组件
    QWidget *centralWidget;
    QHBoxLayout *leftPanelLayout;
    QWidget *navBar;
    QHBoxLayout *rootLayout;
    QWidget *leftPanel;
    SessionManager *sessionManager;
    QScrollArea *chatArea;
    QTextEdit *messageInput;
    QPushButton *sendButton;
    QWidget *inputPanel;
    QVBoxLayout *chatAreaLayout;
    QListWidget *sessionListWidget;
    QString _currentSessionName;

    QWidget *cachedChatPage;    // 新增缓存聊天区域的成员变量
    QWidget *cachedContactsPage;// 新增成员变量
    QWidget *cachedLeftContent;
    QLineEdit *cachedSearchBar;
};


#endif// MAINWIDGET_H