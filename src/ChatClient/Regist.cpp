#include "Client/Regist.h"
#include <QJsonObject>
#include <QRegularExpression>
#include <QScreen>
#include <QTimer>
#include <qlogging.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
Regist::Regist(QWidget *parent)
    : QWidget(parent), ui(new Ui::RegisWidget), isError(true), _verifyCodeTimer(nullptr),_timeLeft(60) {
    ui->setupUi(this);
    setWindowTitle("注册");
    this->setAttribute(Qt::WA_DeleteOnClose);

    this->setWindowFlags(Qt::Window);
    resetStyleSheet();
    // 设置当前的窗口位置为居中
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);
    foreach (QPushButton *button, this->findChildren<QPushButton *>()) {
        button->adjustSize();
    }

    ui->password->setEchoMode(QLineEdit::Password);
    ui->passwordsure->setEchoMode(QLineEdit::Password);

    QTimer::singleShot(0, this, [this]() {
        this->setFixedSize(this->size());
    });

    // 初始化HTTP请求处理器
    initHttpHandlers();

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    connect(ui->passwordsure, &QLineEdit::textChanged, this, &Regist::checkPasswordMatch);
    connect(ui->emailEdit, &QLineEdit::textChanged, this, &Regist::checkEmailFormat);
    connect(ui->pushButton, &QPushButton::clicked, this, [this]() { this->close(); });
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &Regist::slot_reg_mod_finish);
    connect(ui->getVerify, &QPushButton::clicked, this, &Regist::getVarifyCode);
    connect(ui->regisButton, &QPushButton::clicked, this, &Regist::onRegisterButtonClicked);// 添加这一行
}


void Regist::checkPasswordMatch() {
    if (ui->password->text() != ui->passwordsure->text()) {
        m_errorMessages[PasswordMismatch] = "密码不一致";
        setErrorSheet(ui->password);
        setErrorSheet(ui->passwordsure);
    } else {
        m_errorMessages.remove(PasswordMismatch);
        ui->password->setStyleSheet("");
        ui->passwordsure->setStyleSheet("");
    }
    updateErrorMessage();
}

void Regist::checkEmailFormat() {
    auto email = ui->emailEdit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    if (!match) {
        m_errorMessages[EmailFormatError] = "邮箱格式错误";
        setErrorSheet(ui->emailEdit);
    } else {
        m_errorMessages.remove(EmailFormatError);
        ui->emailEdit->setStyleSheet("");
    }
    updateErrorMessage();
}

void Regist::updateErrorMessage() {
    if (m_errorMessages.isEmpty()) {
        ui->message->setText("");
        isError = false;
    } else {
        QStringList errors;
        for (const auto &error: m_errorMessages) {
            errors.append(error);
        }
        ui->message->setText(errors.join("\n"));
        isError = true;
    }
}

void Regist::setErrorSheet(QLineEdit *lineEdit) {
    lineEdit->setStyleSheet("border: 2px solid red;");
}

void Regist::resetStyleSheet() {
    setStyleSheet(
            "QPushButton { background-color: #4CAF50; color: white; border: none; border-radius: 5px; padding: 5px 5px; font-size: 14px; }"
            "QPushButton:hover { background-color: #45a049; }"
            "QLineEdit { border: 2px solid #8fbc8f; border-radius: 7px; padding: 3px; }"
            "QLabel { font-size: 14px; color: #333333; }");

    ui->password->setStyleSheet("");
    ui->passwordsure->setStyleSheet("");
    ui->emailEdit->setStyleSheet("");
}

void Regist::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err) {
    if (err != ErrorCodes::SUCCESSFUL) {
        QMessageBox::warning(this, tr("网络请求错误"), tr("注册失败"));
        return;
    }

    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    //json解析错误
    if (jsonDoc.isNull()) {
        QMessageBox::warning(this, tr("json解析错误"), tr("注册失败"));
        return;
    }

    //json解析错误
    if (!jsonDoc.isObject()) {
        QMessageBox::warning(this, tr("json解析错误"), tr("注册失败"));
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    // 调用对应的逻辑
    if (_handlers.contains(id)) {
        _handlers[id](jsonObj);
    } else {
        qWarning() << "Handler not found for ID:" << id;
    }
    return;
}
void Regist::initHttpHandlers() {
    //注册获取验证码回包逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject jsonObj) {
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESSFUL) {
            QMessageBox::warning(this, tr("参数错误"), tr("获取验证码失败"));
            return;
        }
        auto email = jsonObj["email"].toString();
        QMessageBox::information(this, tr("获取验证码成功"), tr("验证码已发送至邮箱"));
        qDebug() << "email is " << email;
    });

    // 添加注册用户回包逻辑
    _handlers.insert(ReqId::ID_REG_USER, [this](QJsonObject jsonObj) {
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESSFUL) {
            QMessageBox::warning(this, tr("参数错误"), tr("注册失败"));
            return;
        }
        QMessageBox::information(this, tr("注册成功"), tr("注册成功"));
        qDebug() << "User registered successfully";
        this->close();
    });
}

void Regist::getVarifyCode() {
    // 检查定时器是否已经存在
    if (_verifyCodeTimer) {
        _verifyCodeTimer->stop();
        delete _verifyCodeTimer;
    }

    // 创建新的定时器
    _verifyCodeTimer = new QTimer(this);
    _verifyCodeTimer->setInterval(1000); // 1秒

    // 初始化倒计时时间
    _timeLeft = 60;

    connect(_verifyCodeTimer, &QTimer::timeout, this, [this]() {
        if (_timeLeft > 0) {
            ui->getVerify->setText("重新获取验证码(" + QString::number(_timeLeft) + ")");
            _timeLeft--;
        } else {
            _verifyCodeTimer->stop();
            delete _verifyCodeTimer;
            _verifyCodeTimer = nullptr;
            ui->getVerify->setEnabled(true);
            ui->getVerify->setText("获取验证码");
            _timeLeft = 60; // 重置时间
        }
    });

    // 启动定时器
    _verifyCodeTimer->start();

    // 禁用获取验证码按钮
    ui->getVerify->setEnabled(false);

    // 将这个QPushButton的文本设置为"重新获取验证码(60)"
    ui->getVerify->setText("重新获取验证码(" + QString::number(_timeLeft) + ")");

    auto email = ui->emailEdit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    if (match) {
        QJsonObject jsonObj;
        jsonObj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl("http://localhost:8082/get_varifycode"),
                                            jsonObj, ReqId::ID_GET_VARIFY_CODE, Modules::REGISTERMOD);
    } else {
        QMessageBox::warning(this, "警告", "请输入正确的邮箱格式");
    }
}

void Regist::onRegisterButtonClicked() {
    // 获取用户输入
    QString username = ui->userNameEdit->text();
    QString email = ui->emailEdit->text();
    QString verifyCode = ui->verifyCode->text();
    QString password = ui->password->text();
    QString confirmPassword = ui->passwordsure->text();

    QStringList userinput;
    userinput << username << email << verifyCode << password << confirmPassword;

    // 清除之前的错误信息
    m_errorMessages.clear();
    resetStyleSheet();

    //判断是否有误
    if (isError) {
        QMessageBox::critical(this, "错误", "请保证输入正确！");
        return;
    }

    //遍历userinput
    for (int i = 0; i < userinput.size(); i++) {
        QString input = userinput.at(i);
        if (input.isEmpty()) {
            QMessageBox::critical(this, "错误", "请输入完整的注册信息!");
            return;
        }
    }

    // 更新错误信息
    updateErrorMessage();

    // 如果没有错误信息，则执行注册逻辑
    if (m_errorMessages.isEmpty()) {
        // 构建注册请求的 JSON 对象
        QJsonObject jsonObj;
        jsonObj["username"] = username;
        jsonObj["email"] = email;
        jsonObj["verifyCode"] = verifyCode;
        jsonObj["password"] = password;
        jsonObj["passwordsure"] = confirmPassword;

        // 发送注册请求
        HttpMgr::GetInstance()->PostHttpReq(QUrl("http://localhost:8082/user_register"),
                                            jsonObj, ReqId::ID_REG_USER, Modules::REGISTERMOD);
    }
}

Regist::~Regist() {
    delete ui;
}