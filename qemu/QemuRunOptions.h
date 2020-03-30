#ifndef QEMURUNOPTIONS_H
#define QEMURUNOPTIONS_H

#include <QtCore/QtCore>

enum class LaunchMode;

class QemuRunOptions
{
public:
    QemuRunOptions();

    static QemuRunOptions getGlobal();

    void setQmpPort(const QString &port) { qmpPort = port; }
    QString getQmpPort() const { return qmpPort; }

    void setMonitorPort(const QString &port) { monitorPort = port; }
    QString getMonitorPort() const { return monitorPort; }

    void setLogfileName(const QString &name) { logfileName = name; }
    QString getLogfileName() const { return logfileName; }

    void setLogOptionList(const QStringList &options) { logOptionList = options; }
    QStringList getOptionList() const { return logOptionList; }

    void setAdditionalCmdLine(const QString &cmd) { additionalCmdLine = cmd; }
    QString getAdditionalCmdLine() const { return additionalCmdLine; }

    void setDebugEnable(bool value) { isDebugEnable = value; }
    bool getDebugEnable() { return isDebugEnable; }

    void setSnapshotEnable(bool value) { isSnapshotEnable = value; }
    bool getSnapshotEnable() { return isSnapshotEnable; }

    void setQemuRunStopped(bool value) { isQemuRunStopped = value; }
    bool getQemuRunStopped() { return isQemuRunStopped; }

    QString getMonitorCmd();
    QString getQmpCmd();
    QString getAllAdditionalOptionsCmd(LaunchMode mode);

private:
    QString qmpPort;
    QString monitorPort;
    QString logfileName;
    QStringList logOptionList;
    QString additionalCmdLine;
    QString kernel;
    QString initrd;
    bool isDebugEnable;
    bool isSnapshotEnable;
    bool isQemuRunStopped;
};

#endif // QEMURUNOPTIONS_H
