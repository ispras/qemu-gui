#ifndef FILEHELPERS_H
#define FILEHELPERS_H

#include <QtCore/QtCore>

namespace FileHelpers
{
    void deleteDirectory(const QString &path);
    void createDirectory(const QString &path);
};

#endif // FILEHELPERS_H
