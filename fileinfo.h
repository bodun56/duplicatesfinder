#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>


class FileInfo
{
public:
    FileInfo();
    FileInfo(QString file_path, QString file_name, QString file_hash, QString file_data, QString file_size, int file_id);

    void setFilePath(QString value);
    QString getFilePath() const;

    void setFileName(QString value);
    QString getFileName() const;

    void setFileHash(QString value);
    QString getFileHash() const;

    void setFileData(QString value);
    QString getFileData() const;

    void setFileSize(QString value);
    QString getFileSize() const;

    void setFileId(int value);
    int getFileId() const;


private:
    QString filePath;
    QString fileName;
    QString fileHash;
    QString fileData;
    QString fileSize;
    int fileId;

};

#endif // FILEINFO_H
