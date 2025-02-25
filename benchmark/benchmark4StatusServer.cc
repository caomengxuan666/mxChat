/**
 * @FilePath     : /mxChat/src/ChatClient/benchmark4StatusServer.cc
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-24 23:39:34
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include "Client/HttpMgr.h"
#include <QJsonObject>
#include <QString>
#include <Server/config.hpp>

inline static void verifyUser(const QString &username, const QString &password) {
    QJsonObject login_json;
    login_json["user"] = username;
    login_json["passwd"] = password;

    auto cfg = Config_Manager::getInstance();
    cfg.setYamlPath("server.yaml");

    // 发送 HTTP POST 请求
    HttpMgr::GetInstance()->PostHttpReq(
            QUrl("http://localhost:8082/user_login"),
            login_json,
            ReqId::ID_LOGIN_USER,
            Modules::LOGINMOD);
}


void addTest() {
    size_t epoch = 100000;
    const QString username = "cmx";
    const QString password = "123";
    for (int i = 0; i < epoch; ++i) {
        verifyUser(username, password);
    }
}

int main(){
    addTest();
}
