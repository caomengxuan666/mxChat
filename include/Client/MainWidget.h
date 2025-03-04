/**
 * @FilePath     : /mxChat/include/Client/MainWidget.h
 * @Description  :  主要的聊天界面
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-04 17:09:10
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
    void setupUI();
    void setupStyle();
    void addSessionItem(const QString &name, const QString &msg, const QString &time);
    void addMessasge(const QString &sender, const QString &time, const QString &content, MessageType type);

    [[maybe_unused]]
    void addTimeDivider(const QString &timeText);

    QToolButton *createNavButton(const QString &iconName, bool isAvatar = false);
    void onSessionItemClicked(QListWidgetItem *item);
    void updateChatArea(const QString &sessionName);
    // 组件
    QListWidget *sessionList;
    QScrollArea *chatArea;
    QTextEdit *messageInput;
    QPushButton *sendButton;
    QWidget *inputPanel;
    QVBoxLayout *chatAreaLayout;
};


#endif// MAINWIDGET_H