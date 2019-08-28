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
    QStringList getNetdev() const { return netdev; }

private:
    QString path;
    QStringList machines;
    QStringList cpus;
    QStringList netdev;

};


#endif // PLATFORMINFORMATION_H

