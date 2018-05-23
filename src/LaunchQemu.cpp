#include "LaunchQemu.h"


LaunchQemu::LaunchQemu(const QString &qemu_exe, VMConfig *vm, QObject *parent)
    : QObject(parent)
{
    QProcess qemu;
    qemu.start(qemu_exe + " " + vm->get_image_path());
    qemu.waitForFinished();
}

LaunchQemu::~LaunchQemu()
{

}

