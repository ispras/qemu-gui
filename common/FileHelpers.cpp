#include "FileHelpers.h"

namespace FileHelpers {

static void deleteDirectoryRec(const QDir &dir)
{
    int res = 0;
    QStringList dirs = dir.entryList(QDir::Dirs | QDir::AllDirs | QDir::NoDotAndDotDot);
    QStringList files = dir.entryList(QDir::Files);

    foreach(const QString f_name, files)
    {
        QString del_file = dir.absolutePath() + "/" + f_name;
        if (!QFile::remove(del_file))
            qDebug() << "File" << del_file << "was not deleted!";
    }

    foreach(QString d_name, dirs)
    {
        deleteDirectoryRec(QDir(dir.absolutePath() + "/" + d_name));
    }

    QDir().rmdir(dir.absolutePath());
}

void deleteDirectory(const QString &path)
{
    QDir dir(path);
    if (dir.exists())
    {
        deleteDirectoryRec(dir);
    }
}

void createDirectory(const QString &path)
{
    QDir dir(path);
    if (!dir.exists())
    {
        dir.mkpath(path);
    }
}

};
