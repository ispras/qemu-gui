#ifndef PLATFORMINFORMATION_H
#define PLATFORMINFORMATION_H

#include <QtWidgets> 
#include "QMPInteraction.h"
#include "QemuLauncher.h"

class PlatformInformation : public QObject
{
    Q_OBJECT

public:
    PlatformInformation(const QString &qemuPath, const QString &homeDir);
    ~PlatformInformation();

private:
    QString qemuDirPath;
    QString platformDirPath;
    QList<QStringList> platforms;
    QMPInteractionSettings *qmp;
    QemuLauncher *qemu;
    QString qmpPort;
    bool allInfoReady;
    QList<QStringList> result;

private:
    void launchQemu();
    void createXml();

private slots:
    void qmpConnectOk();
    void nextRequest(const QStringList &list);
    void finishQemu(int exitCode);

signals:
    void qmpRequestMachineInfo();
    void qmpRequestCpuInfo();
    void qmpShutdownQemu();

    void workFinish();

};

#endif // PLATFORM_INFORMATION_H
