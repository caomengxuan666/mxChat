/**
 * @FilePath     : /mxChat/include/Client/Regist.h
 * @Description  : 用于控制用户注册的页面
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-04 22:35:29
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

    /**
     * @author       : caomengxuan666
     * @brief        : 错误列表，分为[无错误，密码不匹配，邮箱格式错误]
    **/    
    enum ErrorType {
        NoError,
        PasswordMismatch,
        EmailFormatError
    };

public:
    explicit Regist(QWidget *parent = nullptr);
    ~Regist();

private slots:
    /**
     * @author       : caomengxuan666
     * @brief        : 检查密码是否匹配
     * @return        {*}
    **/
    void checkPasswordMatch();
    /**
     * @author       : caomengxuan666
     * @brief        : 检查邮箱格式是否正确
     * @return        {*}
    **/    
    void checkEmailFormat();
    /**
     * @author       : caomengxuan666
     * @brief        : 处理注册请求的响应
     * @param         {ReqId} id:
     * @param         {QString} res:
     * @param         {ErrorCodes} err:
     * @return        {*}
    **/    
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    /**
     * @author       : caomengxuan666
     * @brief        : 向GateServer发送一个携带了注册信息json的http请求
     * @return        {*}
    **/    
    void getVarifyCode();
private:
    /**
     * @author       : caomengxuan666
     * @brief        : 重置样式
     * @return        {*}
    **/
    void resetStyleSheet();
    /**
     * @author       : caomengxuan666
     * @brief        : 设置不同错误的样式
     * @param         {QLineEdit} *lineEdit:
     * @return        {*}
    **/    
    void setErrorSheet(QLineEdit *lineEdit);
    /**
     * @author       : caomengxuan666
     * @brief        : 更新错误信息
     * @return        {*}
    **/    
    void updateErrorMessage();
    /**
     * @author       : caomengxuan666
     * @brief        : 初始化http请求处理
     * @return        {*}
    **/    
    void initHttpHandlers();
    /**
     * @author       : caomengxuan666
     * @brief        : 注册按钮点击事件
     * @return        {*}
    **/    
    void onRegisterButtonClicked();


    Ui::RegisWidget *ui;
    QMap<ErrorType, QString> m_errorMessages;
    QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;
    bool isError;
    QTimer *_verifyCodeTimer;
    int _timeLeft;  // 添加这个成员变量
};

#endif// REGIST_H
