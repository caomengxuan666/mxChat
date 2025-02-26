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
    void addTestMessage(const QString &sender, const QString &time, const QString &content, MessageType type);
    void addTimeDivider(const QString &timeText);
    void onSessionItemClicked(QListWidgetItem *item);
    void updateChatArea(const QString &sessionName);
    void resizeEvent(QResizeEvent *event);
    // 组件
    QListWidget *sessionList;
    QScrollArea *chatArea;
    QLineEdit *messageInput;
    QPushButton *sendButton;
    QWidget *inputPanel;        // 声明 inputPanel 为成员变量
    QVBoxLayout *chatAreaLayout;// 声明 chatAreaLayout 为成员变量
};


#endif// MAINWIDGET_H