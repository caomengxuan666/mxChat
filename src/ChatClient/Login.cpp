#include <Client/Login.h>
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
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>
#include <qnamespace.h>
#include<QSequentialAnimationGroup>

Login::Login(QWidget *parent) : QWidget(parent, Qt::FramelessWindowHint) {
    setupUI();
    setFixedSize(300, 400);
}

Login::~Login() {}

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
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void Login::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}


void Login::onLoginButtonClicked() {
    // 创建缩小动画
    QPropertyAnimation *shrinkAnimation = new QPropertyAnimation(loginButton, "geometry");
    shrinkAnimation->setDuration(300);  // 增加动画时间
    shrinkAnimation->setStartValue(loginButton->geometry());
    shrinkAnimation->setEndValue(QRect(loginButton->x(), loginButton->y(), loginButton->width() * 0.9, loginButton->height() * 0.9));
    shrinkAnimation->setEasingCurve(QEasingCurve::InOutQuad);  // 更平滑的曲线

    // 创建恢复动画
    QPropertyAnimation *reverseAnimation = new QPropertyAnimation(loginButton, "geometry");
    reverseAnimation->setDuration(300);
    reverseAnimation->setStartValue(QRect(loginButton->x(), loginButton->y(), loginButton->width() * 0.9, loginButton->height() * 0.9));
    reverseAnimation->setEndValue(loginButton->geometry());
    reverseAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    // 使用 QSequentialAnimationGroup 管理动画序列
    QSequentialAnimationGroup *animationGroup = new QSequentialAnimationGroup(this);
    animationGroup->addAnimation(shrinkAnimation);
    animationGroup->addPause(50);  // 添加短暂停顿
    animationGroup->addAnimation(reverseAnimation);
    animationGroup->start(QAbstractAnimation::DeleteWhenStopped);

    // 在动画结束后处理登录逻辑
    connect(animationGroup, &QSequentialAnimationGroup::finished, this, [this]() {
        QString username = usernameLineEdit->text();
        QString password = passwordLineEdit->text();
        if (username == "admin" && password == "123456") {
            emit loginSuccess();  // 发射登录成功信号
            this->hide();
        } else {
            QMessageBox::warning(this, "登录失败", "用户名或密码错误！");
        }
    });
}