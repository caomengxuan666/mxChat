#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QString>

class Server : public QObject {
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr) : QObject(parent) {} // 构造函数直接实现
    virtual ~Server() {}

    virtual void startServer(quint16 port) = 0; // 纯虚函数

signals:
    void messageReceived(QString message); // 信号

protected:
    virtual void onReadyRead() = 0; // 纯虚函数
    virtual void onClientDisconnected() = 0; // 纯虚函数
};

#endif // SERVER_H