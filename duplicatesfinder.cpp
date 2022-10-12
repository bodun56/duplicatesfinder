#include "duplicatesfinder.h"

DuplicatesFinder::DuplicatesFinder(QObject *parent) : QObject(parent)
{
    db.create_DB();
    db.setOnlyFiles(onOnlyFiles);
    connect(&db, &SQLDataBase::onDuplicateFined, this, &DuplicatesFinder::onGetFinedDuplicate);
    connect(&db, &SQLDataBase::onDuplicate, this, &DuplicatesFinder::onGetDuplicates);
}

DuplicatesFinder::~DuplicatesFinder()
{
}

void DuplicatesFinder::setRunning(bool p_running) {
    running = p_running;
    db.dbRemove();
}

void DuplicatesFinder::setMaskFiles(QStringList mask)
{
    maskFilesList.clear();
    for(int i = 0; i < mask.count(); i++)
        if(!mask.at(i).isEmpty())
            maskFilesList.append(mask.at(i));

}

void DuplicatesFinder::setExclude(bool ex)
{
    exclude = ex;
}

void DuplicatesFinder::setOnOnlyFiles(bool onlyFiles)
{
    onOnlyFiles = onlyFiles;
}

void DuplicatesFinder::onGetDuplicates(QStringList list)
{
    emit onDuplicateFined(list);
}

void DuplicatesFinder::checkingFilesStart()
{
    db.setOnlyFiles(onOnlyFiles);
    if(onOnlyFiles){
        checkingFilesStartOnOnlyFiles();
        return;
    }
    if(exclude){
        checkingFilesStartOnExclude();
    }else{
        checkingFilesStartStandart();
    }
}

void DuplicatesFinder::checkingFilesStartStandart()
{
    QFileInfo file;
    QStringList list;
    QDirIterator it(pathFind.path(), maskFilesList, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

    while(it.hasNext()){
        if(!running) return;
        file.setFile(it.next());
        list.append(file.absoluteFilePath());
        emit onPrealodFile(file.baseName());
        emit onPreLoad(list.count());
    }

    if(!list.isEmpty()){
        emit onAllFiles(list.count());
        checkingFiles(list);
    }else{
        db.dbRemove();
        emit finished();
    }
}

void DuplicatesFinder::checkingFilesStartOnExclude()
{
    QFileInfo file;
    QStringList list;
    QStringList listAll;
    QStringList listExclude;
    QDirIterator it(pathFind.path(), QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

    while(it.hasNext()){
        if(!running) return;
        file.setFile(it.next());
        listAll.append(file.absoluteFilePath());
        emit onPrealodFile(file.baseName());
        emit onPreLoad(listAll.count());
    }

    QDirIterator itExclude(pathFind.path(), maskFilesList, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

    while(itExclude.hasNext()){
        if(!running) return;
        file.setFile(itExclude.next());
        listExclude.append(file.absoluteFilePath());
    }

    remover(listAll, listExclude);

    if(!listAll.isEmpty()){
        emit onAllFiles(listAll.count());
        checkingFiles(listAll);
    }else{
        db.dbRemove();
        emit finished();
    }
}

void DuplicatesFinder::remover(QStringList &list, const QStringList &toDelete)
{
    QStringListIterator i(toDelete);
    while(i.hasNext()){
        list.removeAll(i.next());
    }
}

void DuplicatesFinder::checkingFilesStartOnOnlyFiles()
{
    QFileInfo file;
    QStringList list;
    QDirIterator it(pathFind.path(), maskFilesList, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

    while(it.hasNext()){
        if(!running) return;
        file.setFile(it.next());
        list.append(file.absoluteFilePath());
        emit onPrealodFile(file.baseName());
        emit onPreLoad(list.count());
    }

    if(!list.isEmpty()){
        emit onAllFiles(list.count());
        checkingFiles(list);
    }else{
        db.dbRemove();
        emit finished();
    }
}

void DuplicatesFinder::checkingFiles(QStringList list)
{
    QStringList ll;
    QString line;
    count = 0;
    FileInfo fileInfo;
    for(int i=0; i<list.count();i++){

        counterAllFiles++;
        if(!running) {
            db.dbRemove();
            emit finished();
            return;
        }

        QFileInfo finfo = QFileInfo(list.at(i));

        emit onCountAllFilesChanged(counterAllFiles);
        emit onCurrentFile(finfo.fileName());

        fileInfo.setFilePath(finfo.absolutePath());
        fileInfo.setFileName(finfo.fileName());
        if(onOnlyFiles) fileInfo.setFileHash(0);
        else fileInfo.setFileHash(fileChecksum(list.at(i), QCryptographicHash::Md5).toHex());
        fileInfo.setFileSize(fileSized(finfo.size()));
        fileInfo.setFileData(finfo.metadataChangeTime().toString("yyyy.MM.dd hh:mm:ss"));
        fileInfo.setFileId(i);

        QStringList dataInfoList;
        dataInfoList.append(QString::number(fileInfo.getFileId()));
        dataInfoList.append(fileInfo.getFilePath());
        dataInfoList.append(fileInfo.getFileName());
        dataInfoList.append(fileInfo.getFileSize());
        dataInfoList.append(fileInfo.getFileData());
        dataInfoList.append(fileInfo.getFileHash());


        db.insertData(fileInfo);
        emit onListFileInfo(dataInfoList);

    }
    running = false;
    db.duplicateFind();
    db.dbRemove();
    emit finished();
}

QByteArray DuplicatesFinder::fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(hashAlgorithm);

        if (hash.addData(&f)) {
            return hash.result();
        }
    }
    return QByteArray();
}

QString DuplicatesFinder::fileSized(qint64 nSize)
{
    double s = nSize;
    int i = 0;
    for(; s>1023; s /= 1024, i++){
        if(i>4) break;
    }
    return QString().setNum(s) + "BKMGT"[i];
}

bool DuplicatesFinder::isFileExcluded(QString fn, QStringList mask)
{
    fn = fn.toLower();
    for(int i=0;i<mask.count();i++){
        if(fn.toLower().indexOf(mask.at(i).toLower()) >= 0 && !mask.at(i).isEmpty()){
            return true;
        }
    }

    return false;
}

bool DuplicatesFinder::isFileFined(QString fn, QStringList mask)
{
    fn = fn.toLower();
    for(int i=0;i<mask.count();i++){
        if(fn.toLower().indexOf(mask.at(i).toLower()) >= 0 && !mask.at(i).isEmpty()){
            return true;
        }
    }

    return false;
}


