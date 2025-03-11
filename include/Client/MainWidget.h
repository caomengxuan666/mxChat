/**
 * @                       _oo0oo_
 * @                      o8888888o
 * @                      88" . "88
 * @                      (| -_- |)
 * @                      0\  =  /0
 * @                    ___/`---'\___
 * @                  .' \\|     |// '.
 * @                 / \\|||  :  |||// \
 * @                / _||||| -:- |||||- \
 * @               |   | \\\  - /// |   |
 * @               | \_|  ''\---/''  |_/ |
 * @               \  .-\__  '-'  ___/-. /
 * @             ___'. .'  /--.--\  `. .'___
 * @          ."" '<  `.___\_<|>_/___.' >' "".
 * @         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 * @         \  \ `_.   \_ __\ /__ _/   .-` /  /
 * @     =====`-.____`.___ \_____/___.-`___.-'=====
 * @                       `=---='
 * @
 * @
 * @     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @
 * @           佛祖保佑     永不宕机     永无BUG
 * @
 * @       佛曰:  
 * @               写字楼里写字间，写字间里程序员；  
 * @               程序人员写程序，又拿程序换酒钱。  
 * @               酒醒只在网上坐，酒醉还来网下眠；  
 * @               酒醉酒醒日复日，网上网下年复年。  
 * @               但愿老死电脑间，不愿鞠躬老板前；  
 * @               奔驰宝马贵者趣，公交自行程序员。  
 * @               别人笑我忒疯癫，我笑自己命太贱；  
 * @               不见满街漂亮妹，哪个归得程序员？
**/

/**
 * @FilePath     : /mxChat/include/Client/MainWidget.h
 * @Description  :  主要的聊天界面
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-11 16:51:15
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

    void querySearchBar();

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
    void addMessasge(const QString &session_name, const QString &sender, const QString &time, const QString &content, MessageType type);

    /**
    * @author       : caomengxuan666
    * @brief        : 用于创建侧边导航栏
    * @param         {QString} &iconName:
    * @param         {bool} isAvatar:
    * @return        {QToolButton} 侧边导航栏的每一个控件
    **/
    QToolButton *createNavButton(const QString &iconName, bool isAvatar = false);

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
    QString getCurrentSessionName() const;
    
    [[maybe_unused]]
    void addTimeDivider(const QString &timeText);

    // 组件
    SessionManager *sessionManager;
    QScrollArea *chatArea;
    QTextEdit *messageInput;
    QPushButton *sendButton;
    QWidget *inputPanel;
    QVBoxLayout *chatAreaLayout;
    QLineEdit *searchBar;

    QString _currentSessionName;
};


#endif// MAINWIDGET_H