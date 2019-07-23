#include "QemuRunOptions.h"
#include "QemuLauncher.h"

QemuRunOptions::QemuRunOptions() :
    logfileName(""), logOptionList({}), additionalCmdLine(""),
    isDebugEnable(false), isSnapshotEnable(false)
{

}

QString QemuRunOptions::getKernelCmd(const QString &path) const
{
    return (!path.isEmpty()) ? " -kernel " + path : "";
}

QString QemuRunOptions::getInitrdCmd(const QString & path) const
{
    return (!path.isEmpty()) ? " -initrd " + path : "";
}

QString QemuRunOptions::getMonitorCmd()
{
    return " -monitor \"tcp:127.0.0.1:" + getMonitorPort() + ",server,nowait\"";;
}

QString QemuRunOptions::getQmpCmd()
{
    return " -qmp \"tcp:127.0.0.1:" + getQmpPort() + ",server,nowait\"";;
}

QString QemuRunOptions::getAllAdditionalOptionsCmd(LaunchMode mode)
{
    QString debugCmd = (getDebugEnable() && mode != LaunchMode::RECORD) ? " -s -S" : "";
    QString snapshotCmd = (getSnapshotEnable() && mode == LaunchMode::NORMAL) ?
        " -snapshot" : "";
    QString stoppedCmd = (debugCmd.isEmpty() && getQemuRunStopped()) ? " -S" : "";

    QString logOp = "";
    if (!getLogfileName().isEmpty())
    {
        logOp = " -D " + getLogfileName();
    }
    QStringList logOptions = getOptionList();
    if (logOptions.count())
    {
        logOp += " -d ";
        foreach(QString op, logOptions)
        {
            logOp += (op + ",");
        }
        logOp.chop(1);
    }

    return debugCmd + snapshotCmd + stoppedCmd + " " + getAdditionalCmdLine() + logOp;
}



