#include "QemuLauncher.h"
#include "QemuImgLauncher.h"
#include "CommandLineParameters.h"

QemuLauncher::QemuLauncher(const QString &qemu_install_dir_path, VMConfig *vm,
    const QString &port_qmp, const QString &port_monitor, LaunchMode mode,
    const QString &dirRR, QObject *parent)
    : QObject(parent), virtual_machine(vm), port_monitor(port_monitor),
    port_qmp(port_qmp), mode(mode), dirRR(dirRR), qemuDirPath(qemu_install_dir_path)
{
    qemuExePath = qemu_install_dir_path
#ifdef Q_OS_WIN
        + "/" + "qemu-system-" + virtual_machine->getPlatform() + ".exe";
#else
        + "/" + "qemu-system-" + virtual_machine->getPlatform();
#endif
    qemu = NULL;
}

QemuLauncher::~QemuLauncher()
{
}

void QemuLauncher::start_qemu()
{
    CommandLineParameters cmdParams(mode);
    qemu = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(qemu, SIGNAL(finished(int, QProcess::ExitStatus)),
        this, SLOT(finish_qemu(int, QProcess::ExitStatus)));
    mon = " -monitor \"tcp:127.0.0.1:" + port_monitor + ",server,nowait\"";
    qmp = " -qmp \"tcp:127.0.0.1:" + port_qmp + ",server,nowait\"";
    recordReplay = "";
    if (mode != LaunchMode::NORMAL)
    {
        cmdParams.setOverlayDir(dirRR);
        QString rr = mode == LaunchMode::RECORD ? "record" : "replay";
        recordReplay += "-icount shift=7,rr=" + rr + ",rrfile=" +
            "\"" + dirRR + "/replay.bin\"," + "rrsnapshot=init ";
        if (mode == LaunchMode::RECORD)
        {
            cmd = virtual_machine->getCommandLine(cmdParams);
            images = cmdParams.getImages();
            overlays = cmdParams.getOverlays();
            createOverlays();
            qDebug() << "\n\n" << cmd << "\n\n";
        }
    }
    if (mode != LaunchMode::RECORD)
    {
        cmd = virtual_machine->getCommandLine(cmdParams);
        launchQemu();
    }
}

void QemuLauncher::createOverlays()
{
    if (images.isEmpty())
    {
        launchQemu();
    }
    else
    {
        QThread *thread = new QThread();
        QemuImgLauncher *imgLauncher = new QemuImgLauncher(qemuDirPath, "qcow2",
            images.first(), overlays.first());
        images.removeFirst();
        overlays.removeFirst();

        imgLauncher->moveToThread(thread);
        connect(thread, SIGNAL(started()), imgLauncher, SLOT(startQemuImgCreateOverlay()));
        connect(imgLauncher, SIGNAL(qemu_img_finished(int)),
            this, SLOT(finishCreatingOverlay(int)));
        thread->start();
    }
}

void QemuLauncher::launchQemu()
{
    qDebug() << qemuExePath + " " + recordReplay + cmd + mon + qmp;
    qemu->start("\"" + qemuExePath + "\" " + recordReplay + cmd + mon + qmp);
    qemu->waitForFinished(-1);
}

void QemuLauncher::finish_qemu(int exitCode, QProcess::ExitStatus ExitStatus)
{
    qDebug() << "exit code" << exitCode << "exit status" << ExitStatus;
    emit qemu_laucher_finished();
}

void QemuLauncher::finishCreatingOverlay(int exitCode)
{
    if (exitCode == 0)
    {
        createOverlays();
    }
    else
    {
        emit creatingOverlayFailed();
        qDebug() << "Error with creation overlay, sorry";
    }
}
