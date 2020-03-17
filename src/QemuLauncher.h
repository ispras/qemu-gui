#ifndef QEMULAUNCHER_H
#define QEMULAUNCHER_H

#include <QWidget>
#include <QtWidgets>
#include <QTcpSocket>

#include "VMConfig.h"
#include "ConsoleTab.h"
#include "QemuRunOptions.h"
#include "CommandLineParameters.h"

class RecordReplayTab;

class QemuLauncher : public QObject
{
    Q_OBJECT

public:
    QemuLauncher(const QString &qemu_install_dir_path, VMConfig *vm, 
        QemuRunOptions *runOptions, LaunchMode mode, ConsoleTab *console,
        RecordReplayTab *rr, QObject *parent = 0);
    QemuLauncher(const QString &qemuPath, QemuRunOptions *runOptions,
        const QString &platform, const QString &machine);
    ~QemuLauncher();
    bool isQemuExist();

private:
    QemuRunOptions *runOptions;
    QString qemuDirPath;
    QString qemuExePath;
    VMConfig *virtual_machine;
    QProcess *qemu;

    ConsoleTab *con;
    QTcpSocket monitor;
    QString port_qmp;
    QString port_monitor;
    LaunchMode mode;
    QString dirRR;

    QString mon;
    QString qmp;
    QString recordReplay;
    QString cmd;
    QString additionalOptionsCmd;

    QStringList images;
    QStringList overlays;

    QString icount;
    QString period;
    bool overlayRR;
    QString initSnapshot;

private:
    void createQemuPath(const QString &qemuPath, const QString &platform);
    void createOverlays();
    void launchQemu();

private slots:
    void finishCreatingOverlay(int exitCode);

public slots:
    void start_qemu();
    void finish_qemu(int exitCode, QProcess::ExitStatus ExitStatus);
    void terminateQemu();

signals:
    void qemu_laucher_finished(int exitCode);
    void creatingOverlayFailed();
    void noDiskVM();

};


#endif // QEMULAUNCHER_H
