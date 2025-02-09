#include <Client/Login.h>
#include <Client/MainWidget.h>
#include <QApplication>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 创建登录窗口
    Login login;
    login.show();

    // 创建主窗口
    MainWidget mainWidget;

    // 连接登录成功信号到主窗口的槽
    QObject::connect(&login, &Login::loginSuccess, &mainWidget, &MainWidget::onLoginSuccess);

    return app.exec();
}
