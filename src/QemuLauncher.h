#ifndef QEMULAUNCHER_H
#define QEMULAUNCHER_H

#include <QWidget>
#include <QtWidgets>
#include <QTcpSocket>

#include "VMConfig.h"
#include "CommandLineParameters.h"
#include "QemuGUICommon.h"

class QemuLauncher : public QObject
{
    Q_OBJECT

public:
    QemuLauncher(const QString &qemu_install_dir_path, VMConfig *vm, 
        const QString &port_qmp, const QString &port_monitor, LaunchMode mode, QObject *parent = 0);
    ~QemuLauncher();

public:
    void kill_qemu_process();

private:
    QString qemu_dir;
    VMConfig *virtual_machine;
    QProcess *qemu;

    QTcpSocket monitor;
    QString port_qmp;
    QString port_monitor;
    LaunchMode mode;

public slots:
    void start_qemu();
    void finish_qemu(int exitCode, QProcess::ExitStatus ExitStatus);

signals:
    void qemu_laucher_finished();

};


#endif // QEMULAUNCHER_H
