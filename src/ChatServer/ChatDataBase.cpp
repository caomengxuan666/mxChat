// ChatDataBase.cpp
#include "yamlReader.hpp"
#include <DataBase/ChatDataBase.h>
#include <QDebug>
#include <SQLiteCpp/Statement.h>
#include <spdlog/spdlog.h>


ChatDataBase::ChatDataBase(QObject *parent)
    : QObject(parent), m_database(nullptr), m_statement(nullptr) {
}

ChatDataBase::~ChatDataBase() {
    disconnectFromDatabase();
}

bool ChatDataBase::connectToDatabase(const QString &path) {
    try {
        m_database = new SQLite::Database(path.toStdString(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        return true;
    } catch (std::exception &e) {
        qDebug() << "Error connecting to database:" << e.what();
        return false;
    }
}

void ChatDataBase::disconnectFromDatabase() {
    if (m_statement) {
        delete m_statement;
        m_statement = nullptr;
    }
    if (m_database) {
        delete m_database;
        m_database = nullptr;
    }
}

bool ChatDataBase::executeQuery(const QString &query) {
    try {
        m_database->exec(query.toStdString());
        return true;
    } catch (std::exception &e) {
        qDebug() << "Error executing query:" << e.what();
        return false;
    }
}

bool ChatDataBase::executeQueryWithParams(const QString &query, const QVariantList &params) {
    try {
        if (m_statement) {
            delete m_statement;
        }
        m_statement = new SQLite::Statement(*m_database, query.toStdString());

        // 打印原始查询语句和参数
        qDebug() << "Original query:" << query;
        for (const auto &param: params) {
            qDebug() << "Binding parameter:" << param;
        }

        // 绑定参数
        for (int i = 0; i < params.size(); ++i) {
            switch (params[i].typeId()) {
                case QVariant::Int:
                    m_statement->bind(i + 1, params[i].toInt());
                    break;
                case QVariant::String:
                    m_statement->bind(i + 1, params[i].toString().toStdString());
                    break;
                case QVariant::Bool:
                    m_statement->bind(i + 1, params[i].toBool());
                    break;
                default:
                    qDebug() << "Unsupported parameter type:" << params[i].typeName();
                    return false;
            }
        }

        // 打印最终的 SQL 查询语句（调试用）
        qDebug() << "Final query with parameters:" << query;

        // 执行查询并返回结果
        return m_statement->executeStep();
    } catch (std::exception &e) {
        qDebug() << "Error executing query with params:" << e.what();
        return false;
    }
}


bool ChatDataBase::next() {
    try {
        if (m_statement) {
            return m_statement->executeStep();
        }
        return false;
    } catch (std::exception &e) {
        qDebug() << "Error getting next row:" << e.what();
        return false;
    }
}
void ChatDataBase::readConfig() {
    // 调用 YamlReader 读取 YAML 文件
    auto yamlOpt = YamlReader::readYaml("database.yaml");
    if (yamlOpt) {
        spdlog::info("Successfully read YAML file.");

        try {
            // 获取解析后的 YAML 节点
            YAML::Node config = yamlOpt.value();

            // 检查是否存在 "sqlite" 节点
            if (config["sqlite"]) {
                // 提取数据库路径
                if (config["sqlite"]["path"]) {
                    this->dbPath = QString::fromStdString(config["sqlite"]["path"].as<std::string>());
                    spdlog::info("Database path set to: {}", this->dbPath.toStdString());
                } else {
                    spdlog::error("Failed to read 'path' from YAML file.");
                }
            } else {
                spdlog::error("Error reading YAML file: 'database' key not found.");
            }
        } catch (const YAML::Exception &e) {
            spdlog::error("YAML parsing error: {}", e.what());
        } catch (const std::exception &e) {
            spdlog::error("Error processing YAML content: {}", e.what());
        }
    } else {
        spdlog::error("Failed to read YAML file.");
    }
}

QVariant ChatDataBase::value(int index) {
    try {
        if (m_statement) {
            return QVariant::fromValue(m_statement->getColumn(index).getText());
        }
        return QVariant();
    } catch (std::exception &e) {
        qDebug() << "Error getting value:" << e.what();
        return QVariant();
    }
}