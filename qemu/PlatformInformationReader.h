#ifndef PLATFORMINFORMATIONREADER_H
#define PLATFORMINFORMATIONREADER_H

#include <QtCore/QtCore> 
#ifdef GUI
#include <QtWidgets>
#endif
#include "QMPInteraction.h"
#include "QemuLauncher.h"
#include "config/PlatformInfo.h"

class PlatformInformationReader : public QObject
{
    Q_OBJECT

public:
    PlatformInformationReader(const QString &qemu, bool del = true);

private:
    QThread *thread;
#ifdef GUI
    QTimer *timer;
#endif
    QString qemuName;
    QString profilePath;
    QList<QStringList> platforms;
    QMPInteractionSettings *qmp;
    QemuLauncher *launcher;
    bool allInfoReady;
    bool deleteSelf;

    PlatformInfo *platformInfo;

private:
    void launchQemu();
    void createXml();
    void createProgressDialog();

#ifdef GUI
    QProgressDialog *progress;
#endif

private slots:
    void nextRequest();

public slots:
    void timeIsOut();
    void finishQemu(int exitCode);

signals:
    void qmpShutdownQemu();

    void qmpSendCommand();

    void workFinish();
    void qemuMustDie();
};

#endif // PLATFORM_INFORMATIONREADER_H
