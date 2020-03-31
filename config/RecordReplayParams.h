#ifndef RECORDREPLAYPARAMS_H
#define RECORDREPLAYPARAMS_H

#include <QtCore/QtCore>
#include "CommandLineParameters.h"

class RecordReplayParams
{
public:
    RecordReplayParams();

    void createXml() const;
    void readXml();

    QString getCommandLine(LaunchMode mode) const;
    QString getDummyImage() const;

    const QString &getCurrentDir() const { return currentDir; }
    void setCurrentDir(const QString &dir) { currentDir = dir; }
    const QString &getQemu() const { return qemu; }
    void setQemu(const QString &value) { qemu = value; }
    bool isOverlayEnabled() const { return overlay; }
    void setOverlayEnabled(bool value) { overlay = value; }
    int getIcount() const { return icount; }
    void setIcount(int value) { icount = value; }
    int getSnapshotPeriod() const { return snapshotPeriod; }
    void setSnapshotPeriod(int value) { snapshotPeriod = value; }
    const QString &getInitialSnapshot() const { return initialSnapshot; }
    void setInitialSnapshot(const QString &value) { initialSnapshot = value; }

private:
    QString currentDir;
    QString qemu;
    bool overlay;
    int icount;
    int snapshotPeriod;
    QString initialSnapshot;
};

#endif // RECORDREPLAYPARAMS_H
