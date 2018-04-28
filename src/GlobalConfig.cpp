#include "QemuGUI.h"

#include "GlobalConfig.h"
#include <QtWidgets/QApplication>


GlobalConfig::GlobalConfig(QObject *parent)
    : QObject(parent)
{
    path_to_home_dir = QDir::homePath() + "/QemuGUI_VirtualMachines";
    QDir home_dir(path_to_home_dir);
    if (!home_dir.exists())
    {
        home_dir.mkdir(path_to_home_dir);
    }
    vm_config_file = new QFile(path_to_home_dir + "/VMconfig.conf");
    if (!vm_config_file->exists())
    {
        vm_config_file->open(QIODevice::Append);
        vm_config_file->close();
    }
    else
    {
        vm_config_file->open(QIODevice::ReadOnly);
        while (!vm_config_file->atEnd())
        {
            QString path_to_vm = vm_config_file->readLine();
            path_to_vm.chop(1);
            VMConfig *vm = new VMConfig(this, path_to_vm);
            virtual_machines.append(vm);
        }
        vm_config_file->close();
    }
}

GlobalConfig::~GlobalConfig()
{

}


QString GlobalConfig::get_home_dir()
{
    return path_to_home_dir;
}

QList<VMConfig *> GlobalConfig::get_exist_vm()
{
    return virtual_machines;
}

bool GlobalConfig::save_config_file()
{
    if (vm_config_file->open(QIODevice::WriteOnly))
    {
        QTextStream stream(vm_config_file);
        foreach(VMConfig *vm, virtual_machines)
        {
            stream << vm->get_dir_path() << endl;
        }
        vm_config_file->close();
        return true;
    }
    return false;
}

bool GlobalConfig::add_exist_vm(QString path)
{
    VMConfig *vm = new VMConfig(this, path);

    foreach(VMConfig *vm_exist, virtual_machines)
    {
        if (vm->get_name().isEmpty() || vm->get_dir_path().isEmpty())
            return false;
        if (vm_exist->get_name() == vm->get_name() || vm_exist->get_dir_path() == vm->get_dir_path())
            return false;
    }
    virtual_machines.append(vm);

    if (vm_config_file->open(QIODevice::Append))
    {
        QTextStream stream(vm_config_file);
        stream << vm->get_dir_path() << endl;
        vm_config_file->close();
    }
    return true;
}

void GlobalConfig::delete_vm(QString del_vm_name)
{
    QString del_path;

    foreach(VMConfig *vm, virtual_machines)
    {
        if (vm->get_name() == del_vm_name)
        {
            vm->remove_directory_vm();
            virtual_machines.removeOne(vm);
            break;
        }
    }
    save_config_file();
}

void GlobalConfig::exclude_vm(QString del_vm_name)
{
    foreach(VMConfig *vm, virtual_machines)
    {
        if (vm->get_name() == del_vm_name)
        {
            virtual_machines.removeOne(vm);
            break;
        }
    }
    save_config_file();
}

void GlobalConfig::vm_is_created(VMConfig *vm_config)
{
    QString new_vm_path;
    if (!vm_config->get_dir_path().isEmpty())
    {
        new_vm_path = vm_config->get_dir_path();
    }
    else
    {
        new_vm_path = path_to_home_dir + "/" + vm_config->get_name();
    }
    QDir new_vm_dir(new_vm_path);
    if (!new_vm_dir.exists())
    {
        QDir().mkdir(new_vm_path);
    }
    
    if (vm_config->save_vm_config(new_vm_path))
    {
        vm_config->setParent(this);
        virtual_machines.append(vm_config);

        save_config_file();

        emit globalConfig_new_vm_is_complete();
    }
}



