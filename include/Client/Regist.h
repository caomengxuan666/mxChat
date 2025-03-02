/**
 * @FilePath     : /mxChat/include/Client/Regist.h
 * @Description  : 用于控制用户注册的页面
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-02 11:27:52
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#ifndef REGIST_H
#define REGIST_H

#include "Client/HttpMgr.h"
#include "ui_Regist.h"
#include <QLineEdit>
#include <QMap>
#include <QWidget>
#include <qtimer.h>

class Regist : public QWidget {
    Q_OBJECT

    enum ErrorType {
        NoError,
        PasswordMismatch,
        EmailFormatError
    };

public:
    explicit Regist(QWidget *parent = nullptr);
    ~Regist();

private slots:
    void checkPasswordMatch();
    void checkEmailFormat();
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    void getVarifyCode();
private:
    void resetStyleSheet();
    void setErrorSheet(QLineEdit *lineEdit);
    void updateErrorMessage();
    void initHttpHandlers();
    void onRegisterButtonClicked();


    Ui::RegisWidget *ui;
    QMap<ErrorType, QString> m_errorMessages;
    QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;
    bool isError;
    QTimer *_verifyCodeTimer;
    int _timeLeft;  // 添加这个成员变量
};

#endif// REGIST_H
