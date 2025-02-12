#ifndef REGIST_H
#define REGIST_H

#include "Client/HttpMgr.h"
#include "ui_Regist.h"
#include <QLineEdit>
#include <QMap>
#include <QWidget>

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


    Ui::RegisWidget *ui;
    QMap<ErrorType, QString> m_errorMessages;
    QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;
};

#endif// REGIST_H
