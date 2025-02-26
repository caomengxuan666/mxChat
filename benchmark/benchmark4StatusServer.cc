// Benchmark.cpp
#include "Client/HttpMgr.h"
#include <QCoreApplication>
#include <QJsonObject>
#include <QTimer>
#include <atomic>
#include <iostream>
#include <spdlog/spdlog.h>
#include <Server/config.hpp>

class Benchmark : public QObject {
    Q_OBJECT
public:
    Benchmark(QObject* parent = nullptr) 
        : QObject(parent), m_totalRequests(0), m_completed(0) {}

    void startTest(size_t epoch, const QString& user, const QString& pwd) {
        // 初始化配置
        auto& cfg = Config_Manager::getInstance();
        cfg.setYamlPath("server.yaml");

        // 初始化HTTP管理器
        auto httpMgr = HttpMgr::GetInstance();
        connect(httpMgr.get(), &HttpMgr::sig_login_mod_finish,
                this, &Benchmark::handleResponse);

        m_totalRequests = epoch;
        m_startTime = QDateTime::currentDateTime();

        // 批量发送请求
        for(size_t i = 0; i < epoch; ++i) {
            QJsonObject loginJson;
            loginJson["user"] = user;
            loginJson["passwd"] = pwd;

            httpMgr->PostHttpReq(
                QUrl("http://localhost:8082/user_login"),
                loginJson,
                ReqId::ID_LOGIN_USER,
                Modules::LOGINMOD
            );
        }
    }

signals:
    void finished();

private slots:
    void handleResponse(ReqId id, QString res, ErrorCodes err) {
        Q_UNUSED(res)
        
        if(id != ReqId::ID_LOGIN_USER) return;

        if(err != ErrorCodes::SUCCESSFUL) {
            ++m_errorCount[err];
        } else {
            ++m_successCount;
        }

        if(++m_completed >= m_totalRequests) {
            printReport();
            emit finished();
        }
    }

private:
    void printReport() {
        auto duration = m_startTime.msecsTo(QDateTime::currentDateTime());
        
        std::cout << "\n====== Benchmark Report ======\n";
        std::cout << "Total Requests: " << m_totalRequests << "\n";
        std::cout << "Successful: " << m_successCount << "\n";
        std::cout << "Error Distribution:\n";
        for(auto& [code, count] : m_errorCount) {
            std::cout << "  Error " << code << ": " << count << "\n";
        }
        std::cout << "Total Time: " << duration << " ms\n";
        std::cout << "Throughput: " 
                  << (m_totalRequests * 1000.0 / duration) << " req/s\n";
    }

    size_t m_totalRequests;
    std::atomic<size_t> m_completed{0};
    size_t m_successCount{0};
    std::map<ErrorCodes, size_t> m_errorCount;
    QDateTime m_startTime;
};

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    
    const size_t EPOCH = 10000;
    Benchmark benchmark;
    
    QTimer::singleShot(0, [&]() {
        benchmark.startTest(EPOCH, "cmx", "123");
    });

    QObject::connect(&benchmark, &Benchmark::finished, 
                    &app, &QCoreApplication::quit);

    return app.exec();
}

#include "benchmark4StatusServer.moc"