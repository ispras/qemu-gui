#ifndef VMCONFIG_H
#define VMCONFIG_H

#include <QWidget>
#include <QtWidgets>
#include "Device.h"

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
    QString get_vm_info();

    QString get_name();
    QString get_dir_path();
    QString getCommandLine();
    Device *getSystemDevice() { return &system; }

    void remove_directory_vm();
private:
    QFile *list_of_vm_file = NULL;

    QString name_vm;
    QString dir_path;

    Device system;
};

#endif //VMCONFIG_H

