#include "Client/RpcClient.h"// 添加RpcClient头文件
#include <Client/Login.h>
#include <DataBase/DataBaseClient.h>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLinearGradient>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSequentialAnimationGroup>
#include <QShortcut>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>
#include <cstring>
#include <qnamespace.h>
#include <spdlog/spdlog.h>

Login::Login(QWidget *parent) : QWidget(parent, Qt::FramelessWindowHint) {
    setupUI();
    setFixedSize(300, 400);
}

Login::~Login() {
    spdlog::info("Login destructor");
}

void Login::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // 添加顶部退出按钮布局
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setContentsMargins(0, 0, 0, 0);// 右侧留出5像素边距
    exitButton = new QPushButton("×", this);
    exitButton->setFixedSize(20, 20);
    exitButton->setStyleSheet(
            "QPushButton { background-color:rgb(14, 136, 218); color: white; border: none; border-radius: 15px; font-size: 18px; }"
            "QPushButton:hover { background-color:rgb(19, 68, 230); }");

    topLayout->addStretch();
    topLayout->addWidget(exitButton);
    mainLayout->addLayout(topLayout);

    // 初始化其他控件...
    usernameLabel = new QLabel("用户名:", this);
    passwordLabel = new QLabel("密码:", this);
    usernameLineEdit = new QLineEdit(this);
    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton("登录", this);

    // 创建圆形头像标签
    QPixmap avatarPixmap(":/images/avatar.png");
    avatarLabel = new QLabel(this);
    avatarLabel->setPixmap(avatarPixmap.scaled(100, 100, Qt::KeepAspectRatio));
    avatarLabel->setFixedSize(100, 100);
    avatarLabel->setStyleSheet("border-radius: 50%;");
    avatarLabel->setAlignment(Qt::AlignCenter);

    // 设置控件样式（保持原有样式）
    usernameLineEdit->setStyleSheet("QLineEdit { border: 2px solid #8fbc8f; border-radius: 10px; padding: 5px; }");
    passwordLineEdit->setStyleSheet("QLineEdit { border: 2px solid #8fbc8f; border-radius: 10px; padding: 5px; }");
    loginButton->setStyleSheet(
            "QPushButton { background-color: #4CAF50; color: white; border: none; border-radius: 10px; padding: 10px 20px; font-size: 16px; }"
            "QPushButton:hover { background-color: #45a049; }");

    // 布局设置（保持原有结构）
    QHBoxLayout *avatarLayout = new QHBoxLayout();
    avatarLayout->addStretch();
    avatarLayout->addWidget(avatarLabel);
    avatarLayout->addStretch();
    mainLayout->addLayout(avatarLayout);

    mainLayout->addSpacing(20);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(10);
    formLayout->addRow(usernameLabel, usernameLineEdit);
    formLayout->addRow(passwordLabel, passwordLineEdit);
    mainLayout->addLayout(formLayout);

    mainLayout->addSpacing(20);
    mainLayout->addWidget(loginButton);

    // 设置渐变背景
    setGradientBackground();

    // 连接信号槽
    connect(loginButton, &QPushButton::clicked, this, &Login::onLoginButtonClicked);
    connect(exitButton, &QPushButton::clicked, this, &QWidget::close);// 连接退出按钮

    // 绑定Enter shortcut到LoginButton
    QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Enter), this);
    connect(enterShortcut, &QShortcut::activated, this, &Login::onLoginButtonClicked);
}

void Login::setGradientBackground() {
    // 创建线性渐变
    QLinearGradient gradient;
    gradient.setStart(0, 0);
    gradient.setFinalStop(0, height());
    gradient.setColorAt(0, QColor(85, 170, 255));// 起始颜色
    gradient.setColorAt(1, QColor(170, 85, 255));// 结束颜色

    // 设置背景颜色
    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(gradient));
    setAutoFillBackground(true);
    setPalette(palette);
}

void Login::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void Login::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - dragPosition);
        event->accept();
    }
}

void Login::onLoginButtonClicked() {
    // 创建缩小动画
    QPropertyAnimation *shrinkAnimation = new QPropertyAnimation(loginButton, "geometry");
    shrinkAnimation->setDuration(300);// 增加动画时间
    shrinkAnimation->setStartValue(loginButton->geometry());
    shrinkAnimation->setEndValue(QRect(loginButton->x(), loginButton->y(), loginButton->width() * 0.9, loginButton->height() * 0.9));
    shrinkAnimation->setEasingCurve(QEasingCurve::InOutQuad);// 更平滑的曲线

    // 创建恢复动画
    QPropertyAnimation *reverseAnimation = new QPropertyAnimation(loginButton, "geometry");
    reverseAnimation->setDuration(300);
    reverseAnimation->setStartValue(QRect(loginButton->x(), loginButton->y(), loginButton->width() * 0.9, loginButton->height() * 0.9));
    reverseAnimation->setEndValue(loginButton->geometry());
    reverseAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    // 使用 QSequentialAnimationGroup 管理动画序列
    QSequentialAnimationGroup *animationGroup = new QSequentialAnimationGroup(this);
    animationGroup->addAnimation(shrinkAnimation);
    animationGroup->addPause(50);// 添加短暂停顿
    animationGroup->addAnimation(reverseAnimation);
    animationGroup->start(QAbstractAnimation::DeleteWhenStopped);

    // 在动画结束后处理登录逻辑
    connect(animationGroup, &QSequentialAnimationGroup::finished, this, [this]() {
        QString username = usernameLineEdit->text();
        QString password = passwordLineEdit->text();
        bool isValid = verifyUser(username, password);
        if (isValid) {
            emit loginSuccess();// 发射登录成功信号
            this->hide();
        } else {
            QMessageBox::warning(this, "登录失败", "用户名或密码错误！");
        }
    });
}

bool Login::verifyUser(const QString &username, const QString &password) {
    // 创建RpcClient实例
    RpcClient rpcClient;
    rpcClient.connectToServer("127.0.0.1", 8080);// 假设服务器地址和端口

    // 构造请求参数
    QString method = "login";
    QString params = username + "," + password;// 简单拼接，实际应用中可能需要更复杂的参数构造

    // 创建事件循环
    QEventLoop loop;
    bool isValid = false;

    // 连接信号以获取响应
    connect(&rpcClient, &RpcClient::messageReceived, this, [this, &loop, &isValid](const QString &response) {
        qDebug() << "Message received!";

        if (std::strcmp(response.toStdString().c_str(), "Login successful") == 0) {
            isValid = true;
        }

        qDebug() << "\n\nResponse: " << response;
        qDebug() << "isValid: " << isValid;

        // 确保在接收到响应后退出事件循环
        QMetaObject::invokeMethod(&loop, &QEventLoop::quit, Qt::QueuedConnection);
    });

    // 发送请求并获取响应
    rpcClient.sendMessage(method, params);

    // 等待响应
    loop.exec();

    return isValid;
}
