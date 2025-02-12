#ifndef LOGIN_H
#define LOGIN_H

#include <Client/Client.h>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLinearGradient>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <qcolor.h>
#include <qnamespace.h>
#include <qpoint.h>
#include <qpushbutton.h>

class Login : public QWidget {
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private:
    void setupUI();
    void setGradientBackground();
    bool verifyUser(const QString &username, const QString &password);

signals:
    void loginSuccess();

private slots:
    void onLoginButtonClicked();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);


private:
    QLabel *usernameLabel;
    QLabel *passwordLabel;
    QLineEdit *usernameLineEdit;
    QLineEdit *passwordLineEdit;
    QPushButton *loginButton;
    QPushButton *exitButton;
    QLabel *avatarLabel;
    QPoint dragPosition;
    QColor m_gradientStart;
    QColor m_gradientEnd;
    double m_gradientAngle;
    QTimer m_gradientTimer;
};

#endif// LOGIN_H