// DataBaseClient.h
#ifndef DATABASECLIENT_H
#define DATABASECLIENT_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>

class DataBaseClient : public QObject
{
    Q_OBJECT
public:
    explicit DataBaseClient(QObject *parent = nullptr);
    ~DataBaseClient();

    bool connectToDatabase(const QString &path);
    void disconnectFromDatabase();
    bool executeQuery(const QString &query);
    bool executeQueryWithParams(const QString &query, const QVariantList &params);
    bool next();
    QVariant value(int index);

private:
    SQLite::Database *m_database;
    SQLite::Statement *m_statement;
};

#endif // DATABASECLIENT_H