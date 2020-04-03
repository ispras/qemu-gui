#ifndef PLATFORMINFORMATION_H
#define PLATFORMINFORMATION_H

#include <QtCore/QtCore>

class PlatformInfo
{
public:
    explicit PlatformInfo(const QString &path);

    const QStringList &getMachines() const { return machines; }
    const QStringList &getCpus() const { return cpus; }
    const QStringList &getNetdev() const { return netdev; }

    void addMachine(const QString &s, bool isDefault = false);
    void addCpu(const QString &s);
    void addNetdev(const QString &s);

    void saveXml() const;

private:
    QString path;
    QStringList machines;
    QStringList cpus;
    QStringList netdev;
};


#endif // PLATFORMINFORMATION_H

