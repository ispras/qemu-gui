#include "QemuLauncher.h"
#include "CommandLineParameters.h"

QemuLauncher::QemuLauncher(const QString &qemu_install_dir_path, VMConfig *vm,
    const QString &port_qmp, const QString &port_monitor, LaunchMode mode,
    const QString &dirRR, QObject *parent)
    : QObject(parent), virtual_machine(vm), port_monitor(port_monitor),
    port_qmp(port_qmp), mode(mode), dirRR(dirRR)
{
    qemu_dir = qemu_install_dir_path
#ifdef Q_OS_WIN
        + "/qemu-system-i386w.exe";
#else
        + "/qemu-system-i386";
#endif
    qemu = NULL;
}

QemuLauncher::~QemuLauncher()
{
}

void QemuLauncher::kill_qemu_process()
{
    qemu->kill();
    emit qemu->finished(0, QProcess::ExitStatus::NormalExit);
}


void QemuLauncher::start_qemu()
{
    CommandLineParameters cmdParams(mode);
    qemu = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(qemu, SIGNAL(finished(int, QProcess::ExitStatus)), 
        this, SLOT(finish_qemu(int, QProcess::ExitStatus)));
    QString mon = " -monitor \"tcp:127.0.0.1:" + port_monitor + ",server,nowait\"";
    QString qmp = " -qmp \"tcp:127.0.0.1:" + port_qmp + ",server,nowait\"";
    QString recordReplay = "";
    if (mode != LaunchMode::NORMAL)
    {
        QString rr = mode == LaunchMode::RECORD ? "record" : "replay";
        recordReplay = "-icount shift=7,rr=" + rr + ",rrfile=" + 
            dirRR + "/replay.bin";
    }
    qDebug() << qemu_dir + " " + recordReplay + virtual_machine->getCommandLine(cmdParams) + mon + qmp;
    qemu->start("\"" + qemu_dir + "\" " + recordReplay +
        virtual_machine->getCommandLine(cmdParams) + mon + qmp);
    qemu->waitForFinished(-1);
}

void QemuLauncher::finish_qemu(int exitCode, QProcess::ExitStatus ExitStatus)
{
    emit qemu_laucher_finished();
}
