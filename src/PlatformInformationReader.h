#ifndef PLATFORMINFORMATIONREADER_H
#define PLATFORMINFORMATIONREADER_H

#include <QtWidgets> 
#include "QMPInteraction.h"
#include "QemuLauncher.h"

class PlatformInformationReader : public QObject
{
    Q_OBJECT

public:
    PlatformInformationReader(const QString &qemuPath, const QString &homeDir);
    ~PlatformInformationReader();
    static QString getQemuProfilePath(const QString &name);
    static QString getQemuHash(const QString &name);

private:
    QThread *thread;
    QProgressDialog *progress;
    QString qemuDirPath;
    QString platformDirPath;
    QString currentPlatformDirPath;
    QList<QStringList> platforms;
    QMPInteractionSettings *qmp;
    QemuLauncher *qemu;
    QString qmpPort;
    bool allInfoReady;
    QList<QStringList> result;
    QTimer *timer;

private:
    void launchQemu();
    void createProgressDialog();
    void createXml();

private slots:
    void qmpConnectOk();
    void nextRequest(const QStringList &list);

public slots:
    void timeIsOut();
    void finishQemu(int exitCode);

signals:
    void qmpRequestMachineInfo();
    void qmpRequestCpuInfo();
    void qmpShutdownQemu();

    void workFinish();
    void qemuMustDie();

};

#endif // PLATFORM_INFORMATIONREADER_H
