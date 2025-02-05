// MainWidget.h
#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPainter>
#include <QPushButton>
#include <QScrollBar>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <qtmetamacros.h>
#include <QPropertyAnimation>

enum class MessageType {
    Self,  // 用户自己发送的消息
    Other, // 其他人发送的消息
    Image, // 图片消息
    File,  // 文件消息
    System,// 系统消息
};

class MainWidget : public QWidget {
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);

public slots:
    void onLoginSuccess();

private:
    void setupUI();
    void setupStyle();
    void addSessionItem(const QString &name, const QString &msg, const QString &time);
    void addChatMessage(const QString &sender,
                        const QString &time,
                        const QString &content,
                        MessageType type);
    void startAnimation();
    void addTimeDivider(const QString &timeText);
    void onSessionItemClicked(QListWidgetItem *item);
    void updateChatArea(const QString &sessionName);
    // 组件
    QListWidget *sessionList;
    QTextEdit *chatArea;
    QLineEdit *messageInput;
    QPushButton *sendButton;
    QWidget *inputPanel;// 声明 inputPanel 为成员变量
    
    QPropertyAnimation *windowOpacityAnimation;
};


#endif// MAINWIDGET_H