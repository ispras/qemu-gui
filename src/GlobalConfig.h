#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include "VMConfig.h"
#include <QtWidgets>


class GlobalConfig : public QObject
{
    Q_OBJECT

public:
    GlobalConfig(QObject *parent = 0);
    ~GlobalConfig();

    QString get_home_dir();
    QList<VMConfig *> get_exist_vm();
    VMConfig *get_vm_by_name(const QString &name);
    
    bool add_exist_vm(const QString &path);
    void delete_exclude_vm(const QString &del_vm_name, bool delete_vm);
    void exclude_vm(const QString &del_vm_name);
    void set_qemu_dirs(const QString &qemu_exe, bool isAdd);
    QStringList & get_qemu_dirs();

private:
    QList<VMConfig *> virtual_machines;
    QStringList qemu_list;
    QFile *vm_config_file;
    QString path_to_home_dir;

private:
    bool save_config_file();

public slots:
    void vm_is_created(VMConfig *vm_config);


signals:
    void globalConfig_new_vm_is_complete();

};


#endif // GLOBALCONFIG_H

