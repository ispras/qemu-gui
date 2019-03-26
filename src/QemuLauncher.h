#ifndef QEMULAUNCHER_H
#define QEMULAUNCHER_H

#include <QWidget>
#include <QtWidgets>
#include <QTcpSocket>

#include "VMConfig.h"


enum class LaunchMode : int { NORMAL, RECORD, REPLAY };

class QemuLauncher : public QObject
{
    Q_OBJECT

public:
    QemuLauncher(const QString &qemu_install_dir_path, VMConfig *vm, 
        const QString &port_qmp, const QString &port_monitor, LaunchMode mode,
        bool isDebugEnable, const QString &dirRR, const QString &icount,
        const QString &periodSnap, QObject *parent = 0);
    QemuLauncher(const QString &qemuPath, const QString &platform,
        const QString &machine, const QString &port_qmp);
    ~QemuLauncher();
    bool isQemuExist();

private:
    QString qemuDirPath;
    QString qemuExePath;
    VMConfig *virtual_machine;
    QProcess *qemu;

    QTcpSocket monitor;
    QString port_qmp;
    QString port_monitor;
    LaunchMode mode;
    QString dirRR;

    QString mon;
    QString qmp;
    QString recordReplay;
    QString cmd;
    QString debugCmd;

    QStringList images;
    QStringList overlays;

    QString icount;
    QString period;
    bool isDebug;

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

};


#endif // QEMULAUNCHER_H
