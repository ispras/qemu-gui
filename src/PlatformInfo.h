#ifndef PLATFORMINFORMATION_H
#define PLATFORMINFORMATION_H

#include <QtWidgets>

class PlatformInfo : public QObject
{

public:
    explicit PlatformInfo(const QString &path);
    ~PlatformInfo() {}

    const QStringList &getMachines() const { return machines; }
    const QStringList &getCpus() const { return cpus; }
    const QStringList &getNetdev() const { return netdev; }

private:
    QString path;
    QStringList machines;
    QStringList cpus;
    QStringList netdev;

};


#endif // PLATFORMINFORMATION_H

