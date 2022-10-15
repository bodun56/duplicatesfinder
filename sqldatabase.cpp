#include "sqldatabase.h"

SQLDataBase::SQLDataBase()
{
    dbInit();
}

void SQLDataBase::dbInit()
{
    if(db.isOpen()) return;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");

    if(!db.open()) qInfo() << "db is open FAIL";

    query = QSqlQuery(db);
}

SQLDataBase::~SQLDataBase()
{

}

void SQLDataBase::create_DB()
{
    QStringList list;
    list.append("create table if not exists tab_all (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, full_path text, file_name text, hash text, file_date text, file_size text)");
    list.append("create table if not exists tab_duplicates (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, full_path text, file_name text, hash text, file_date text, file_size text)");
    list.append("DELETE FROM tab_all");
    list.append("DELETE FROM tab_duplicates");

    for(int i = 0; i < list.count(); i++){
        if(!query.exec(list.at(i))){
            qInfo() << "Error db: " << query.lastError().text();
        }
    }
}

void SQLDataBase::insertData(FileInfo data)
{
    query.prepare("INSERT INTO tab_all (full_path, file_name, hash, file_date, file_size) VALUES (:full_path, :file_name, :hash, :file_date, :file_size)");
    query.bindValue(":full_path", data.getFilePath());
    query.bindValue(":file_name", data.getFileName());
    query.bindValue(":hash", data.getFileHash());
    query.bindValue(":file_date", data.getFileData());
    query.bindValue(":file_size", data.getFileSize());

    if(!query.exec()){
        qInfo() << query.lastError().text();
        qInfo() << query.lastQuery();
    }
}

void SQLDataBase::duplicateFind()
{
    int count = -1;
    QString sql = "select count(id) from tab_all";
    if(onlyFiles){
        sql = "select * from tab_all";
        QList<FileInfo> file_info;
        if(query.exec(sql)){
            while(query.next()){
                FileInfo f_info;
                f_info.setFileId(query.value(0).toInt());
                f_info.setFilePath(query.value(1).toString());
                f_info.setFileName(query.value(2).toString());
                f_info.setFileHash(query.value(3).toString());
                f_info.setFileData(query.value(4).toString());
                f_info.setFileSize(query.value(5).toString());

                file_info.append(f_info);

                QStringList list;
                list.append(query.value("id").toString());
                list.append(query.value("full_path").toString());
                list.append(query.value("file_name").toString());
                list.append(query.value("file_size").toString());
                list.append(query.value("file_date").toString());
                list.append(query.value("hash").toString());
                emit onDuplicate(list);
            }
        }else{
            qInfo() << query.lastError().text();
            qInfo() << query.lastQuery();
            return;
        }
        insertDuplicatedInTables(file_info);
        return;
    }

    if(query.exec(sql)){
        while(query.next()){
            count = query.value(0).toInt();
        }
    }else{
        qInfo() << query.lastError().text();
        qInfo() << query.lastQuery();
        return;
    }

    if(count <= 0){
        return;
    }

    sql = "select hash from tab_all";
    QStringList hashes;
    if(query.exec(sql)){
        while(query.next()){
            hashes.append(query.value(0).toString());
        }
    }else{
        qInfo() << "duplicateFind: error(1)";
    }

    int duplicateCount;
    for(int i = 0; i<count;i++){
        duplicateCount = 0;
        query.prepare("select count(id) from tab_all where hash = :hash");
        query.bindValue(":hash", hashes.at(i));
        if(query.exec()){
            while(query.next()){
                duplicateCount = query.value(0).toInt();
            }
        }else{
            qInfo() << query.lastError().text();
            qInfo() << query.lastQuery();
        }

        if(duplicateCount > 1){
            QList<FileInfo> file_info;
            query.prepare("select * from tab_all where hash = :hash");
            query.bindValue(":hash", hashes.at(i));
            if(query.exec()){
                while(query.next()){

                    FileInfo f_info;
                    f_info.setFileId(query.value(0).toInt());
                    f_info.setFilePath(query.value(1).toString());
                    f_info.setFileName(query.value(2).toString());
                    f_info.setFileHash(query.value(3).toString());
                    f_info.setFileData(query.value(4).toString());
                    f_info.setFileSize(query.value(5).toString());

                    file_info.append(f_info);

                    QStringList list;
                    list.append(query.value("id").toString());
                    list.append(query.value("full_path").toString());
                    list.append(query.value("file_name").toString());
                    list.append(query.value("file_size").toString());
                    list.append(query.value("file_date").toString());
                    list.append(query.value("hash").toString());
                    emit onDuplicate(list);
                }
            }else{
                qInfo() << "duplicateFind: error(3)";
                qInfo() << query.lastError().text();
                qInfo() << query.lastQuery();
            }
            insertDuplicatedInTables(file_info);
            file_info.clear();
        }
    }

}

void SQLDataBase::setOnlyFiles(bool val)
{
    onlyFiles = val;
}

void SQLDataBase::insertDuplicatedInTables(QList<FileInfo> list)
{
    for(int i=0;i<list.count();i++){
        query.prepare("INSERT INTO tab_duplicates (full_path, file_name, hash, file_date, file_size) VALUES (:full_path, :file_name, :hash, :file_date, :file_size)");
        query.bindValue(":full_path", list.at(i).getFilePath());
        query.bindValue(":file_name", list.at(i).getFileName());
        query.bindValue(":hash", list.at(i).getFileHash());
        query.bindValue(":file_date", list.at(i).getFileData());
        query.bindValue(":file_size", list.at(i).getFileSize());

        if(!query.exec()){
            qInfo() << "Error insert data";
            qInfo() << query.lastError().text();
            qInfo() << query.lastQuery();
        }
    }

    if(!onlyFiles){
        for(int i = 0;i<list.count();i++){
            query.prepare("delete from tab_all where hash = :hash");
            query.bindValue(":hash", list.at(i).getFileHash());
            if(!query.exec()){
                qInfo() << "Error delete hash";
            }
        }
    }
}


