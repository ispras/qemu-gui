#ifndef PLATFORMINFORMATION_H
#define PLATFORMINFORMATION_H

#include <QtWidgets>

class PlatformInformation : public QObject
{
public:
    PlatformInformation() {}
    ~PlatformInformation() {}

    static QStringList getPlatformInfoByName(const QString &path, 
        const QString &element);

};


#endif // PLATFORMINFORMATION_H

