#ifndef QEMULAUNCHER_H
#define QEMULAUNCHER_H

#include <QtCore/QtCore>
#include <QTcpSocket>

#include "VMConfig.h"
#include "QemuRunOptions.h"
#include "CommandLineParameters.h"
#include "RecordReplayParams.h"

class RecordReplayTab;

class QemuLauncher : public QObject
{
    Q_OBJECT

public:
    QemuLauncher(const QString &qemu_install_dir_path, VMConfig *vm, 
        const QemuRunOptions &runOpt, LaunchMode mode,
        const RecordReplayParams &rr, QObject *parent = 0);
    QemuLauncher(const QString &qemuPath, const QemuRunOptions &runOpt,
        const QString &platform, const QString &machine);
    ~QemuLauncher();
    bool isQemuExist();

private:
    QemuRunOptions runOptions;
    QString qemuDirPath;
    QString qemuExePath;
    VMConfig *virtual_machine;
    QProcess *qemu;

    QTcpSocket monitor;
    QString port_qmp;
    QString port_monitor;
    LaunchMode mode;

    QString mon;
    QString qmp;
    QString recordReplay;
    QString cmd;
    QString additionalOptionsCmd;

    QStringList images;
    QStringList overlays;

    RecordReplayParams rrParams;

private:
    void createQemuPath(const QString &qemuPath, const QString &platform);
    void createDummyImage();
    void createOverlays();
    void launchQemu();

private slots:
    void finishCreatingOverlay(int exitCode);
    void finishCreatingDummy(int exitCode);

public slots:
    void start_qemu();
    void finish_qemu(int exitCode, QProcess::ExitStatus ExitStatus);
    void terminateQemu();

signals:
    void qemu_laucher_finished(int exitCode);
    void creatingOverlayFailed();
    void qemuStarted(const QString &cmdline);
};


#endif // QEMULAUNCHER_H
