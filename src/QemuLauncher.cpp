#include "QemuLauncher.h"
#include "QemuImgLauncher.h"
#include "CommandLineParameters.h"
#include "RecordReplayTab.h"

QemuLauncher::QemuLauncher(const QString &qemu_install_dir_path, VMConfig *vm,
    QemuRunOptions *runOptions, LaunchMode mode, ConsoleTab *console, 
    RecordReplayTab *rr, QObject *parent)
    : QObject(parent), virtual_machine(vm), mode(mode), dirRR(""),
    qemuDirPath(qemu_install_dir_path), icount(""), period(""),
    con(console), runOptions(runOptions), initSnapshot("init")
{
    createQemuPath(qemu_install_dir_path, virtual_machine->getPlatform());
    qemu = NULL;
    mon = runOptions->getMonitorCmd();
    qmp = runOptions->getQmpCmd();
    additionalOptionsCmd = runOptions->getAllAdditionalOptionsCmd(mode);
    if (rr)
    {
        dirRR = rr->getCurrentDirRR();
        icount = rr->getICountValue();
        overlayRR = rr->isOverlayEnabled();
        period = overlayRR ? rr->getSnapshotPeriod() : "";
        if (mode == LaunchMode::REPLAY)
        {
            initSnapshot = rr->getInitSnapshot();
        }
        connect(this, SIGNAL(noDiskVM()), rr, SLOT(noDiskVM()));
    }
}

QemuLauncher::QemuLauncher(const QString &qemuPath, QemuRunOptions *runOptions,
    const QString &platform, const QString &machine)
    : mode(LaunchMode::NORMAL), con(NULL), runOptions(runOptions),
    overlayRR(false), initSnapshot("")
{
    createQemuPath(qemuPath, platform);
    cmd = "-machine " + machine + " ";
    qemu = NULL;
    virtual_machine = NULL;
    mon = "";
    qmp = runOptions->getQmpCmd();
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
        cmdParams.setOverlayDir(dirRR);
        cmdParams.setOverlayEnabled(overlayRR);
        QString initSnapshotCmd = overlayRR ? ",rrsnapshot=" + initSnapshot : "";
        QString rr = mode == LaunchMode::RECORD ? "record" : "replay";
        recordReplay += "-icount shift=" + icount + ",rr=" + rr + ",rrfile=" +
            "\"" + dirRR + "/replay.bin\"" + initSnapshotCmd;
        if (mode == LaunchMode::RECORD)
        {
            cmd = virtual_machine->getCommandLine(cmdParams);
            images = cmdParams.getImages();
            if (images.size() == 0)
            {
                emit noDiskVM();
                launchQemu();
            }
            else
            {
                overlays = cmdParams.getOverlays();
                createOverlays();
            }
        }
        if (!period.isEmpty())
        {
            recordReplay += ",rrperiod=" + period;
        }
    }
    if (mode != LaunchMode::RECORD)
    {
        if (virtual_machine)
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
    QString kernelInitrd = "";
    if (virtual_machine)
    {
        kernelInitrd += runOptions->getKernelCmd(virtual_machine->getKernel());
        kernelInitrd += runOptions->getInitrdCmd(virtual_machine->getInitrd());
    }
    QString cmdLine = "\"" + qemuExePath + "\" " + recordReplay
        + kernelInitrd + cmd + mon + runOptions->getQmpCmd() + additionalOptionsCmd;
    qDebug() << cmdLine;
    if (con)
    {
        con->addConsoleText(cmdLine);
    }
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
