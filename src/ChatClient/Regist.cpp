#include "Client/Regist.h"

#include <QJsonObject>
#include <QRegularExpression>
#include <QScreen>
#include <QTimer>
#include <qmessagebox.h>
#include <qpushbutton.h>
Regist::Regist(QWidget *parent)
    : QWidget(parent), ui(new Ui::RegisWidget) {
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
    connect(ui->getVerify,&QPushButton::clicked,this,&Regist::getVarifyCode);
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
    } else {
        QStringList errors;
        for (const auto &error: m_errorMessages) {
            errors.append(error);
        }
        ui->message->setText(errors.join("\n"));
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
    if (err != ErrorCodes::SUCCESS) {
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

    //调用对应的逻辑

    _handlers[id](jsonDoc.object());
    return;
}
void Regist::initHttpHandlers() {
    //注册获取验证码回包逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject jsonObj) {
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            QMessageBox::warning(this, tr("参数错误"), tr("获取验证码失败"));
            return;
        }
        auto email = jsonObj["email"].toString();
        QMessageBox::information(this, tr("获取验证码成功"), tr("验证码已发送至邮箱"));
        qDebug() << "email is " << email;
    });
}

void Regist::getVarifyCode() {
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

Regist::~Regist() {
    delete ui;
}