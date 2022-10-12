#include "fileinfo.h"


FileInfo::FileInfo()
{

}

FileInfo::FileInfo(QString file_path, QString file_name, QString file_hash, QString file_data, QString file_size, int file_id)
{
    filePath = file_path;
    fileName = file_name;
    fileHash = file_hash;
    fileSize = file_size;
    fileData = file_data;
    fileId = file_id;
}


void FileInfo::setFilePath(QString value)
{
    filePath = value;
}

QString FileInfo::getFilePath() const
{
    return filePath;
}

void FileInfo::setFileName(QString value)
{
    fileName = value;
}

QString FileInfo::getFileName() const
{
    return fileName;
}

void FileInfo::setFileHash(QString value)
{
    fileHash = value;
}

QString FileInfo::getFileHash() const
{
    return fileHash;
}

void FileInfo::setFileData(QString value)
{
    fileData = value;
}

QString FileInfo::getFileData() const
{
    return fileData;
}

void FileInfo::setFileSize(QString value)
{
    fileSize = value;
}

QString FileInfo::getFileSize() const
{
    return fileSize;
}

void FileInfo::setFileId(int value)
{
    fileId = value;
}

int FileInfo::getFileId() const
{
    return fileId;
}
