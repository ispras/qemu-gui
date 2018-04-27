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
    QStringList get_exist_vm();
    QString get_created_vm_name();
    
    bool add_exist_vm(QString path);
    void delete_vm(QString del_vm_name);
    void exclude_vm(QString del_vm_name);

private:
    QList<VMConfig *> virtual_machines;
    

public slots:
    void vm_is_created(VMConfig *vm_config);


private:
    QFile *vm_config_file;
    QString path_to_home_dir;

signals:
    void globalConfig_new_vm_is_complete();

};


#endif // GLOBALCONFIG_H

