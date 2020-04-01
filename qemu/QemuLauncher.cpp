#include "QemuLauncher.h"
#include "QemuImgLauncher.h"
#include "CommandLineParameters.h"
#include "config/QemuList.h"

QemuLauncher::QemuLauncher(const QString &qemu, VMConfig *vm,
    const QemuRunOptions &runOpt, LaunchMode mode,
    const RecordReplayParams &rr, QObject *parent)
    : QObject(parent), virtual_machine(vm), mode(mode),
    qemuName(qemu), runOptions(runOpt), rrParams(rr)
{
    createQemuPath(virtual_machine->getPlatform());
    process = NULL;
    mon = runOptions.getMonitorCmd();
    qmp = runOptions.getQmpCmd();
    additionalOptionsCmd = runOptions.getAllAdditionalOptionsCmd(mode);
}

QemuLauncher::QemuLauncher(const QString &qemu, const QemuRunOptions &runOpt,
    const QString &platform, const QString &machine)
    : qemuName(qemu), mode(LaunchMode::NORMAL), runOptions(runOpt)
{
    createQemuPath(platform);
    cmd = "-machine " + machine + " ";
    process = NULL;
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

void QemuLauncher::createQemuPath(const QString &platform)
{
    qemuExePath = QemuList::getQemuExecutablePath(qemuName, platform);
}

void QemuLauncher::start_qemu()
{
    CommandLineParameters cmdParams(mode);
    process = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)),
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
    QemuImgLauncher *imgLauncher = new QemuImgLauncher(QemuList::getQemuDir(qemuName), "qcow2",
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
        QemuImgLauncher *imgLauncher = new QemuImgLauncher(QemuList::getQemuDir(qemuName), "qcow2",
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
    process->start(cmdLine);
#ifdef GUI
    if (virtual_machine)
    {
        process->waitForFinished(-1);
    }
    else
    {
        process->waitForFinished(10000);
    }
#endif
}

void QemuLauncher::finish_qemu(int exitCode, QProcess::ExitStatus ExitStatus)
{
    qDebug() << "exit code" << exitCode << "exit status" << ExitStatus;
    emit qemu_laucher_finished(exitCode);
}

void QemuLauncher::terminateQemu()
{
    if (process->state() == QProcess::Running)
    {
        qDebug() << "Qemu work too long. Terminated. Receiving information is not guaranteed";
        process->terminate();
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
