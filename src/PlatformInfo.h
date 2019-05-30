#ifndef PLATFORMINFORMATION_H
#define PLATFORMINFORMATION_H

#include <QtWidgets>

class PlatformInfo : public QObject
{

public:
    PlatformInfo(const QString &path);
    ~PlatformInfo() {}

    QStringList getMachines() { return machines; }
    QStringList getCpus() { return cpus; }

private:
    QStringList machines;
    QStringList cpus;

};


#endif // PLATFORMINFORMATION_H

