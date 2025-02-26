#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QString>


class Client : public QObject {
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr){}
    virtual ~Client() {}

    virtual void connectToServer(const QString &address, quint16 port) = 0;
    virtual void sendMessage(const QString &method, const QString &message) = 0;

signals:
    void messageReceived(QString message);
    void connected();
    void errorOccurred(QString errorString);

protected:
    virtual void onReadyRead() = 0;
    virtual void onConnected() = 0;
    virtual void onDisconnected() = 0;
    virtual void onError() = 0;
};

#endif // CLIENT_H