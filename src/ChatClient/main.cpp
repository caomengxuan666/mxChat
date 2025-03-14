#include <Client/Login.h>
#include <Client/MainWidget.h>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
int main(int argc, char *argv[]) {
    // 设置缩放比例，1080P为1.0基准
    qputenv("QT_SCALE_FACTOR", QByteArray::number(1.5));

// 如果qt版本小于5.6，开启高DPI支持
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif

    QApplication::setHighDpiScaleFactorRoundingPolicy(
            Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

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