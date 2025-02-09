// MessageWidget.h
#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QDateTime>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

enum class MessageType {
    Self,  // 用户自己发送的消息
    Other, // 其他人发送的消息
    Image, // 图片消息
    File,  // 文件消息
    System,// 系统消息
};


class MessageWidget : public QWidget {
    Q_OBJECT
public:
    explicit MessageWidget(const QString &sender,
                           const QString &time,
                           const QString &content,
                           MessageType type,
                           QWidget *parent = nullptr);

private:
    void setupUI(const QString &sender, const QString &time, const QString &content, MessageType type);
};

#endif// MESSAGEWIDGET_H