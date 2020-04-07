#include "QemuRunOptions.h"
#include "CommandLineParameters.h"
#include "config/GlobalConfig.h"

QemuRunOptions::QemuRunOptions() :
    logfileName(""), logOptionList({}), additionalCmdLine(""),
    isDebugEnable(false), isSnapshotEnable(false), isQemuRunStopped(false)
{
}

QemuRunOptions QemuRunOptions::getGlobal()
{
    QemuRunOptions opt;
    opt.qmpPort = GlobalConfig::get_port_qmp();
    opt.monitorPort = GlobalConfig::get_port_monitor();
    return opt;
}

QString QemuRunOptions::getMonitorCmd()
{
    if (getMonitorPort().isEmpty())
    {
        return "";
    }

    return " -monitor \"tcp:127.0.0.1:" + getMonitorPort() + ",server,nowait\"";
}

QString QemuRunOptions::getQmpCmd()
{
    if (getQmpPort().isEmpty())
    {
        return "";
    }

    return " -qmp \"tcp:127.0.0.1:" + getQmpPort() + ",server,nowait\"";
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



