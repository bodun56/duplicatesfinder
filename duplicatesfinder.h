#ifndef DUPLICATESFINDER_H
#define DUPLICATESFINDER_H


#include "fileinfo.h"
#include "sqldatabase.h"

#include <QObject>
#include <QThread>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QFile>
#include <QDialog>
#include <QFileSystemModel>
#include <QDirIterator>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>

class DuplicatesFinder : public QObject {
    Q_OBJECT
public:
    explicit DuplicatesFinder(QObject *parent = nullptr);
    ~DuplicatesFinder();
    void setPathFind(QString path){pathFind.setPath(path);};
    QString getPathFind(){return pathFind.path();};
    bool running;
    void setRunning(bool p_running);;
    bool getRunning(){return running;};
    void setMaskFiles(QStringList mask);
    void setExclude(bool ex);
    void setOnOnlyFiles(bool onlyFiles);
    void checkingFilesStart();

    void onGetDuplicates(QStringList list);



private:
    QDir pathFind;
    QStringList listFiles;
    unsigned long count;
    void checkingFiles(QStringList);
    QByteArray fileChecksum(const QString &fileName,
                            QCryptographicHash::Algorithm hashAlgorithm);
    QString fileSized(qint64 nSize);
    SQLDataBase db;
    int counterAllFiles = 0;
    int counterDuplicates = 0;
    QStringList maskFilesList;
    bool exclude;
    bool onOnlyFiles;
    bool isFileExcluded(QString fn, QStringList mask);
    bool isFileFined(QString fn, QStringList mask);
    /**/
    void checkingFilesStartStandart();
    void checkingFilesStartOnExclude();
    void remover(QStringList &list, const QStringList &toDelete);
    void checkingFilesStartOnOnlyFiles();

signals:
    void finished();
    QStringList onListFiles();
    FileInfo onLatestFile(FileInfo fInfo);
    QStringList onListFileInfo(QStringList list);
    QString onTest(QString test);
    void onStarted();
    unsigned long onCountChanged(unsigned long count);
    FileInfo onFinedDuplicate(FileInfo);
    QStringList onDuplicateFined(QStringList);
    int onCountAllFilesChanged(int);
    int onCountDuplicatesChanged(int);
    int onAllFiles(int);
    QString onCurrentFile(QString);
    int onPreLoad(int);
    QString onPrealodFile(QString);
    FileInfo onGetFinedDuplicate(FileInfo dup);

};

#endif // DUPLICATESFINDER_H
