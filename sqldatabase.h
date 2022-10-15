#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include "fileinfo.h"
#include <QObject>
#include <QtSql>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTemporaryFile>
#include <QDebug>

class SQLDataBase : public QObject
{
    Q_OBJECT
public:
    SQLDataBase();
    ~SQLDataBase();
    void create_DB();   //создание баз
    void insertData(FileInfo data);
    void duplicateFind();
    void setOnlyFiles(bool val);


private:
    QSqlDatabase db;
    QSqlQuery query;
    bool onlyFiles;
    void dbInit();
    void insertDuplicatedInTables(QList<FileInfo> list);

signals:
    FileInfo onDuplicateFined(FileInfo duplicate);
    QStringList onDuplicate(QStringList duplicate);

};

#endif // SQLDATABASE_H
