#include "QemuLauncher.h"


QemuLauncher::QemuLauncher(const QString &qemu_install_dir_path, VMConfig *vm, QObject *parent)
    : QObject(parent)
{
    qemu_dir = qemu_install_dir_path
#ifdef Q_OS_WIN
        + "/qemu-system-i386w.exe";
#else
        + "/qemu-system-i386";
#endif
    virtual_machine = vm;
}

QemuLauncher::~QemuLauncher()
{
}


void QemuLauncher::start_qemu()
{
    QProcess *qemu = new QProcess();
    connect(qemu, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finish_qemu(int, QProcess::ExitStatus)));
    qemu->start(qemu_dir + " " + virtual_machine->get_image_path());
    qemu->waitForFinished(-1);
}

void QemuLauncher::finish_qemu(int exitCode, QProcess::ExitStatus ExitStatus)
{
    emit qemu_laucher_finished();
}

