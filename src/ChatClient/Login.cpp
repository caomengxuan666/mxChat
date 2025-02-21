#include "Client/HttpMgr.h"
#include <Client/Login.h>
#include <Client/Regist.h>
#include <QDesktopServices>
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
#include <Server/config.hpp>
#include <cstring>
#include <qdebug.h>
#include <qjsonobject.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <spdlog/spdlog.h>


class RainbowBorder : public QWidget {
    Q_OBJECT                                          // Required for Qt's meta-object system
    Q_PROPERTY(int angle READ getAngle WRITE setAngle)// Declare the property

            public : RainbowBorder(QWidget *parent = nullptr) : QWidget(parent), m_angle(0) {
        setAttribute(Qt::WA_TransparentForMouseEvents);

        // Set up the animation
        m_animation = new QPropertyAnimation(this, "angle");
        m_animation->setDuration(2000);
        m_animation->setStartValue(0);
        m_animation->setEndValue(360);
        m_animation->setLoopCount(-1);// Loop indefinitely
        m_animation->start();         // Start the animation
    }

    void setAngle(int angle) {
        if (m_angle != angle) {
            m_angle = angle;
            update();// Trigger a repaint when the angle changes
        }
    }

    int getAngle() const {
        return m_angle;
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        int border = 6;// Border width
        QRectF rect = QRectF(border, border, width() - 2 * border, height() - 2 * border);

        // Create a conical gradient
        QConicalGradient gradient(rect.center(), m_angle);
        gradient.setColorAt(0.0, Qt::red);
        gradient.setColorAt(0.142, QColor(255, 165, 0));// Orange
        gradient.setColorAt(0.285, Qt::yellow);
        gradient.setColorAt(0.428, Qt::green);
        gradient.setColorAt(0.571, Qt::blue);
        gradient.setColorAt(0.714, QColor(75, 0, 130));   // Indigo
        gradient.setColorAt(0.857, QColor(238, 130, 238));// Violet
        gradient.setColorAt(1.0, Qt::red);

        QPen pen;
        pen.setWidth(border);
        pen.setBrush(QBrush(gradient));
        painter.setPen(pen);

        // Draw the ellipse
        painter.drawEllipse(rect);
    }

private:
    int m_angle;                    // Current angle of the gradient
    QPropertyAnimation *m_animation;// Animation for the rainbow effect
};
Login::Login(QWidget *parent) : QWidget(parent, Qt::FramelessWindowHint) {
    setupUI();
    initHttpHandlers();

    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_login_mod_finish, this,
            &Login::slot_login_mod_finish);
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
    topLayout->setContentsMargins(0, 0, 0, 0);
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

    QPushButton *registerButton = new QPushButton("注册", this);
    registerButton->setStyleSheet(
            "QPushButton { background-color: #4CAF50; color: white; border: none; border-radius: 10px; padding: 10px 20px; font-size: 16px; }"
            "QPushButton:hover { background-color: #45a049; }");
    loginButton->setFixedSize(100, 40);   // 设置登录按钮的固定大小为 100x40
    registerButton->setFixedSize(100, 40);// 设置注册按钮的固定大小为 100x40

    // 创建圆形头像标签
    QPixmap avatarPixmap(":/images/avatar.png");
    QWidget *avatarContainer = new QWidget(this);
    avatarContainer->setFixedSize(112, 112);
    avatarContainer->setStyleSheet("background: transparent;");

    // 先创建彩虹边框
    RainbowBorder *rainbowBorder = new RainbowBorder(avatarContainer);
    rainbowBorder->resize(avatarContainer->size());

    // 后创建头像标签
    avatarLabel = new QLabel(avatarContainer);
    avatarPixmap = avatarPixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    avatarLabel->setPixmap(avatarPixmap);
    avatarLabel->setFixedSize(100, 100);
    avatarLabel->move(6, 6);                                // 居中显示
    avatarLabel->setAttribute(Qt::WA_TranslucentBackground);// 设置透明背景

    // 设置圆形遮罩
    QRegion maskRegion(QRect(0, 0, 100, 100), QRegion::Ellipse);
    avatarLabel->setMask(maskRegion);

    // 设置控件样式（保持原有样式）
    usernameLineEdit->setStyleSheet("QLineEdit { border: 2px solid #8fbc8f; border-radius: 10px; padding: 5px; }");
    passwordLineEdit->setStyleSheet("QLineEdit { border: 2px solid #8fbc8f; border-radius: 10px; padding: 5px; }");
    loginButton->setStyleSheet(
            "QPushButton { background-color: #4CAF50; color: white; border: none; border-radius: 10px; padding: 10px 20px; font-size: 16px; }"
            "QPushButton:hover { background-color: #45a049; }");

    // 布局设置（保持原有结构）
    QHBoxLayout *avatarLayout = new QHBoxLayout();
    avatarLayout->addStretch();
    avatarLayout->addWidget(avatarContainer);
    avatarLayout->addStretch();
    mainLayout->addLayout(avatarLayout);

    mainLayout->addSpacing(20);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(10);
    formLayout->addRow(usernameLabel, usernameLineEdit);
    formLayout->addRow(passwordLabel, passwordLineEdit);

    // 添加“忘记密码”标签到表单布局的最后一行
    QLabel *forgetPasswordLabel = new QLabel("<a href=\"http://127.0.0.1:11451/index\">忘记密码</a>", this);
    forgetPasswordLabel->setStyleSheet("QLabel { color: #4CAF50; }");
    forgetPasswordLabel->setAlignment(Qt::AlignRight);
    formLayout->addRow(forgetPasswordLabel);
    connect(forgetPasswordLabel, &QLabel::linkActivated, [this](const QString &link) {
        //在浏览器中打开
        QDesktopServices::openUrl(QUrl(link));
    });

    mainLayout->addLayout(formLayout);

    mainLayout->addSpacing(20);

    // 创建水平布局来容纳登录和注册按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();// 左侧弹簧
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(registerButton);
    buttonLayout->addStretch();// 右侧弹簧

    mainLayout->addLayout(buttonLayout);

    // 设置当前的窗口位置为居中
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);

    // 设置渐变背景
    setGradientBackground();

    // 连接信号槽
    connect(loginButton, &QPushButton::clicked, this, &Login::onLoginButtonClicked);
    connect(exitButton, &QPushButton::clicked, this, &QWidget::close);// 连接退出按钮

    connect(registerButton, &QPushButton::clicked, this, [this]() {
        // 隐藏当前窗口
        this->hide();

        // 创建并显示注册窗口
        Regist *regist = new Regist(this);
        connect(regist, &QWidget::destroyed, this, &QWidget::show);// 注册窗口关闭时显示登录窗口
        regist->show();
    });

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
    gradient.setColorAt(1, QColor(170, 85, 200));// 结束颜色

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
    bool isValid = false;

    QJsonObject login_json;
    login_json["user"] = username;
    login_json["passwd"] = password;
    auto host = Config_Manager::getInstance();

    HttpMgr::GetInstance()
            ->PostHttpReq(QUrl("/user_login"),
                          login_json, ReqId::ID_LOGIN_USER, Modules::LOGINMOD);
    isValid = true;
    return isValid;
}

void Login::initHttpHandlers() {
    //注册获取登录回包逻辑
    _handlers.insert(ReqId::ID_LOGIN_USER, [this](QJsonObject jsonObj) {
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESSFUL) {
            return;
        }
        auto user = jsonObj["user"].toString();
    });
}

void Login::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err) {
    if (err != ErrorCodes::SUCCESSFUL) {
        return;
    }

    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    //json解析错误
    if (jsonDoc.isNull()) {
        return;
    }

    //json解析错误
    if (!jsonDoc.isObject()) {
        return;
    }

    //调用对应的逻辑,根据id回调。
    _handlers[id](jsonDoc.object());

    return;
}
#include "Login.moc"
