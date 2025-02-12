// ChatDataBase.h
#ifndef ChatDataBase_H
#define ChatDataBase_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>

class ChatDataBase : public QObject
{
    Q_OBJECT
    friend class RpcServer;
public:
    explicit ChatDataBase(QObject *parent = nullptr);
    ~ChatDataBase();

    bool connectToDatabase(const QString &path);
    void disconnectFromDatabase();
    bool executeQuery(const QString &query);
    bool executeQueryWithParams(const QString &query, const QVariantList &params);
    bool next();
    void readConfig();
    QVariant value(int index);

private:
    SQLite::Database *m_database;
    SQLite::Statement *m_statement;
    QString dbPath;
};

#endif // ChatDataBase_H