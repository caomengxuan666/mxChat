// DataBaseClient.cpp
#include <DataBase/DataBaseClient.h>
#include <QDebug>
#include <SQLiteCpp/Statement.h>

DataBaseClient::DataBaseClient(QObject *parent)
    : QObject(parent), m_database(nullptr), m_statement(nullptr)
{
}

DataBaseClient::~DataBaseClient()
{
    disconnectFromDatabase();
}

bool DataBaseClient::connectToDatabase(const QString &path)
{
    try {
        m_database = new SQLite::Database(path.toStdString(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        return true;
    } catch (std::exception &e) {
        qDebug() << "Error connecting to database:" << e.what();
        return false;
    }
}

void DataBaseClient::disconnectFromDatabase()
{
    if (m_statement) {
        delete m_statement;
        m_statement = nullptr;
    }
    if (m_database) {
        delete m_database;
        m_database = nullptr;
    }
}

bool DataBaseClient::executeQuery(const QString &query)
{
    try {
        m_database->exec(query.toStdString());
        return true;
    } catch (std::exception &e) {
        qDebug() << "Error executing query:" << e.what();
        return false;
    }
}

bool DataBaseClient::executeQueryWithParams(const QString &query, const QVariantList &params)
{
    try {
        if (m_statement) {
            delete m_statement;
        }
        m_statement = new SQLite::Statement(*m_database, query.toStdString());

        // 打印原始查询语句和参数
        qDebug() << "Original query:" << query;
        for (const auto &param : params) {
            qDebug() << "Binding parameter:" << param;
        }

        // 绑定参数
        for (int i = 0; i < params.size(); ++i) {
            switch (params[i].type()) {
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


bool DataBaseClient::next()
{
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

QVariant DataBaseClient::value(int index)
{
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