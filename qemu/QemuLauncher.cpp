#include "QemuLauncher.h"
#include "QemuImgLauncher.h"
#include "CommandLineParameters.h"

QemuLauncher::QemuLauncher(const QString &qemu_install_dir_path, VMConfig *vm,
    const QemuRunOptions &runOpt, LaunchMode mode,
    const RecordReplayParams &rr, QObject *parent)
    : QObject(parent), virtual_machine(vm), mode(mode),
    qemuDirPath(qemu_install_dir_path),
    runOptions(runOpt), rrParams(rr)
{
    createQemuPath(qemu_install_dir_path, virtual_machine->getPlatform());
    qemu = NULL;
    mon = runOptions.getMonitorCmd();
    qmp = runOptions.getQmpCmd();
    additionalOptionsCmd = runOptions.getAllAdditionalOptionsCmd(mode);
}

QemuLauncher::QemuLauncher(const QString &qemuPath, const QemuRunOptions &runOpt,
    const QString &platform, const QString &machine)
    : mode(LaunchMode::NORMAL), runOptions(runOpt)
{
    createQemuPath(qemuPath, platform);
    cmd = "-machine " + machine + " ";
    qemu = NULL;
    virtual_machine = NULL;
    mon = "";
    qmp = runOptions.getQmpCmd();
    additionalOptionsCmd = "";
}

QemuLauncher::~QemuLauncher()
{
}

bool QemuLauncher::isQemuExist()
{
    QFile qemuFile(qemuExePath);
    return qemuFile.exists();
}

void QemuLauncher::createQemuPath(const QString &qemuPath, const QString &platform)
{
    qemuExePath = qemuPath
#ifdef Q_OS_WIN
        + "/" + "qemu-system-" + platform + "w.exe";
#else
        + "/" + "qemu-system-" + platform;
#endif
}

void QemuLauncher::start_qemu()
{
    CommandLineParameters cmdParams(mode);
    qemu = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(qemu, SIGNAL(finished(int, QProcess::ExitStatus)),
        this, SLOT(finish_qemu(int, QProcess::ExitStatus)));
    recordReplay = "";
    if (mode != LaunchMode::NORMAL)
    {
        cmdParams.setOverlayDir(rrParams.getCurrentDir());
        cmdParams.setOverlayEnabled(rrParams.isOverlayEnabled());
        QString rr = rrParams.getCommandLine(mode);
        recordReplay += rr;
        if (mode == LaunchMode::RECORD)
        {
            cmd = virtual_machine->getCommandLine(cmdParams);
            images = cmdParams.getImages();
            overlays = cmdParams.getOverlays();
            createDummyImage();
        }
    }
    if (mode != LaunchMode::RECORD)
    {
        if (virtual_machine)
            cmd = virtual_machine->getCommandLine(cmdParams);
        launchQemu();
    }
}

void QemuLauncher::createDummyImage()
{
    QThread *thread = new QThread();
    QemuImgLauncher *imgLauncher = new QemuImgLauncher(qemuDirPath, "qcow2",
        rrParams.getDummyImage(), 20);

    imgLauncher->moveToThread(thread);
    connect(thread, SIGNAL(started()), imgLauncher, SLOT(startQemuImgCreateDisk()));
    connect(imgLauncher, SIGNAL(qemu_img_finished(int)),
         this, SLOT(finishCreatingDummy(int)));
    thread->start();
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
    QString cmdLine = "\"" + qemuExePath + "\" " + recordReplay
        + cmd + mon + runOptions.getQmpCmd() + additionalOptionsCmd;
    qDebug() << cmdLine;
    emit qemuStarted(cmdLine);
    qemu->start(cmdLine);
    if (virtual_machine)
    {
        qemu->waitForFinished(-1);
    }
    else
    {
        qemu->waitForFinished(10000);
    }
}

void QemuLauncher::finish_qemu(int exitCode, QProcess::ExitStatus ExitStatus)
{
    qDebug() << "exit code" << exitCode << "exit status" << ExitStatus;
    emit qemu_laucher_finished(exitCode);
}

void QemuLauncher::terminateQemu()
{
    if (qemu->state() == QProcess::Running)
    {
        qDebug() << "Qemu work too long. Terminated. Receiving information is not guaranteed";
        qemu->terminate();
    }
}

void QemuLauncher::finishCreatingDummy(int exitCode)
{
    if (exitCode == 0)
    {
        createOverlays();
    }
    else
    {
        emit creatingOverlayFailed();
        qDebug() << "Error with dummy disk image, sorry";
    }
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
