#ifndef QEMURUNOPTIONS_H
#define QEMURUNOPTIONS_H

#include <QtWidgets>

enum class LaunchMode;

class QemuRunOptions : public QObject
{
    Q_OBJECT

public:
    QemuRunOptions();

public:
    void setQmpPort(const QString &port) { qmpPort = port; }
    QString getQmpPort() { return qmpPort; }

    void setMonitorPort(const QString &port) { monitorPort = port; }
    QString getMonitorPort() { return monitorPort; }

    void setLogfileName(const QString &name) { logfileName = name; }
    QString getLogfileName() { return logfileName; }

    void setLogOptionList(const QStringList &options) { logOptionList = options; }
    QStringList getOptionList() { return logOptionList; }

    void setAdditionalCmdLine(const QString &cmd) { additionalCmdLine = cmd; }
    QString getAdditionalCmdLine() { return additionalCmdLine; }

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
    bool isDebugEnable;
    bool isSnapshotEnable;
    bool isQemuRunStopped;
};

#endif // QEMURUNOPTIONS_H
