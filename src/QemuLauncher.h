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
        const QString &dirRR, QObject *parent = 0);
    ~QemuLauncher();

private:
    QString qemu_dir;
    VMConfig *virtual_machine;
    QProcess *qemu;

    QTcpSocket monitor;
    QString port_qmp;
    QString port_monitor;
    LaunchMode mode;
    QString dirRR;

public slots:
    void start_qemu();
    void finish_qemu(int exitCode, QProcess::ExitStatus ExitStatus);

signals:
    void qemu_laucher_finished();

};


#endif // QEMULAUNCHER_H
