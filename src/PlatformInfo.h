#ifndef PLATFORMINFORMATION_H
#define PLATFORMINFORMATION_H

#include <QtWidgets>

class PlatformInfo : public QObject
{

public:
    explicit PlatformInfo(const QString &path);
    ~PlatformInfo() {}

    QStringList getMachines() const { return machines; }
    QStringList getCpus() const { return cpus; }

private:
    QStringList machines;
    QStringList cpus;

};


#endif // PLATFORMINFORMATION_H

