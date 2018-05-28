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
    QProcess qemu;
    qemu.start(qemu_dir + " " + virtual_machine->get_image_path());
    qemu.waitForFinished(-1);
}

