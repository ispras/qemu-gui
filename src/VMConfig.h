#ifndef VMCONFIG_H
#define VMCONFIG_H

#include <QWidget>
#include <QtWidgets>
#include "Device.h"
#include "DeviceUsb.h"

class CommandLineParameters;

class VMConfig : public QObject
{
    Q_OBJECT

public:
    VMConfig(QObject *parent, const QString &path);
    ~VMConfig();

public:
    bool save_vm_config(const QString &path) const;
    void save_vm_config() const;
    void createVMFolder(const QString &path) const;
    void set_name(const QString &name_vm_);
    void addDefaultIDE(const QString &image);
    void addMemorySize(const QString &size);
    void addUsbDevice();
    QString get_vm_info();

    QString get_name();
    QString get_dir_path();
    QString getCommandLine(CommandLineParameters &cmdParams);
    Device *getSystemDevice() { return &system; }
    QStringList getReplayList() { return replayList; }
    void fillReplayList();

    void remove_directory_vm();

private:
    QFile *list_of_vm_file = NULL;

    QString name_vm;
    QString dir_path;

    Device system;
    QStringList replayList;

};

#endif //VMCONFIG_H

