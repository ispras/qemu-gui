#ifndef PLATFORMINFORMATIONREADER_H
#define PLATFORMINFORMATIONREADER_H

#include <QtCore/QtCore> 
#ifdef GUI
#include <QtWidgets>
#endif
#include "QMPInteraction.h"
#include "QemuLauncher.h"

class PlatformInformationReader : public QObject
{
    Q_OBJECT

public:
    PlatformInformationReader(const QString &qemu, const QString &profile, bool del = true);

private:
    QThread *thread;
    QString qemuPath;
    QString profilePath;
    QList<QStringList> platforms;
    QMPInteractionSettings *qmp;
    QemuLauncher *launcher;
    bool allInfoReady;
    QList<QStringList> result;
    QTimer *timer;
    bool deleteSelf;

private:
    void launchQemu();
    void createXml();
    void createProgressDialog();

#ifdef GUI
    QProgressDialog *progress;
#endif

private slots:
    void qmpConnectOk();
    void nextRequest(const QStringList &list, bool isReady);

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
