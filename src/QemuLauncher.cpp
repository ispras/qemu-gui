#include "QemuLauncher.h"
#include "QemuImgLauncher.h"
#include "CommandLineParameters.h"
#include "RecordReplayTab.h"

QemuLauncher::QemuLauncher(const QString &qemu_install_dir_path, VMConfig *vm,
    QemuRunOptions *runOptions, LaunchMode mode, ConsoleTab *console, 
    RecordReplayTab *rr, QObject *parent)
    : QObject(parent), virtual_machine(vm), mode(mode), dirRR(""),
    qemuDirPath(qemu_install_dir_path), icount(""), period(""),
    con(console)
{
    createQemuPath(qemu_install_dir_path, virtual_machine->getPlatform());
    qemu = NULL;
    mon = " -monitor \"tcp:127.0.0.1:" + runOptions->getMonitorPort() + ",server,nowait\"";
    qmp = " -qmp \"tcp:127.0.0.1:" + runOptions->getQmpPort() + ",server,nowait\"";
    
    QString debugCmd = (runOptions->getDebugEnable() && mode != LaunchMode::RECORD) ? " -s -S" : "";
    QString snapshotCmd = (runOptions->getSnapshotEnable() && mode == LaunchMode::NORMAL) ?
        " -snapshot" : "";
    con->addConsoleText(snapshotCmd);
    QString logOp = "";
    if (!runOptions->getLogfileName().isEmpty())
    {
        logOp = " -D " + runOptions->getLogfileName();
    }
    QStringList logOptions = runOptions->getOptionList();
    if (logOptions.count())
    {
        logOp += " -d ";
        foreach(QString op, logOptions)
        {
            logOp += (op + ",");
        }
        logOp.chop(1);
    }
    
    additionalOptionsCmd = debugCmd + snapshotCmd + " "
        + runOptions->getAdditionalCmdLine() + logOp;

    if (rr)
    {
        dirRR = rr->getCurrentDirRR();
        icount = rr->getICountValue();
        period = rr->getSnapshotPeriod();
    }
}

QemuLauncher::QemuLauncher(const QString &qemuPath, const QString &platform,
    const QString &machine, const QString &port_qmp)
    : port_qmp(port_qmp), mode(LaunchMode::NORMAL), con(NULL)
{
    createQemuPath(qemuPath, platform);
    cmd = "-machine " + machine + " ";
    qemu = NULL;
    virtual_machine = NULL;
    mon = "";
    qmp = " -qmp \"tcp:127.0.0.1:" + port_qmp + ",server,nowait\"";
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
        + "/" + "qemu-system-" + platform + ".exe";
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
        QString rr = mode == LaunchMode::RECORD ? "record" : "replay";
        recordReplay += "-icount shift=" + icount + ",rr=" + rr + ",rrfile=" +
            "\"" + dirRR + "/replay.bin\"," + "rrsnapshot=init";
        if (mode == LaunchMode::RECORD)
        {
            cmd = virtual_machine->getCommandLine(cmdParams);
            images = cmdParams.getImages();
            overlays = cmdParams.getOverlays();
            createOverlays();
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
    QString cmdLine = "\"" + qemuExePath + "\" " + recordReplay + " -net none "
        + cmd + mon + qmp + additionalOptionsCmd;
    qDebug() << cmdLine;
    con->addConsoleText(cmdLine);
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
