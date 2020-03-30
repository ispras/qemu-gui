#ifndef VMCONFIG_H
#define VMCONFIG_H

#include <QtCore/QtCore>

#include "Device.h"

class CommandLineParameters;

class VMConfig
{
public:
    VMConfig(const QString &path);
    ~VMConfig();

public:
    void readVMConfig();
    bool save_vm_config(const QString &path) const;
    void save_vm_config() const;
    void createVMFolder(const QString &path) const;
    void set_name(const QString &name_vm_);
    void setCmdLine(const QString &cmdLine);
    void addDefaultBus(const QString &image);
    void addDeviceMemory(const QString &size);
    void addDeviceMachine(const QString &name);
    void addDeviceCpu(const QString &name);
    void setKernel(const QString &name);
    void setInitrd(const QString &name);
    void addUsbDevice();
    QString get_vm_info();

    void setPlatform(const QString &platformVM);
    QString getPlatform();
    QString getMachine();

    QString get_name();
    QString getKernel();
    QString getInitrd();
    QString getCmdLine();
    QString get_dir_path();
    QString getCommandLine(CommandLineParameters &cmdParams);
    Device *getSystemDevice() { return &system; }
    QString getPathRRDir();
    QStringList getReplayList() { return replayList; }
    void fillReplayList();

    void remove_directory_vm();

private:
    QFile *list_of_vm_file;

    QString path;
    QString name_vm;
    QString kernel;
    QString initrd;
    QString dir_path;
    QString platform;
    QString addCmdLine;
    Device system;
    QStringList replayList;

};

#endif //VMCONFIG_H

