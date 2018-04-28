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

bool GlobalConfig::add_exist_vm(const QString &path)
{
    VMConfig *vm = new VMConfig(this, path);

    if (vm->get_name().isEmpty() || vm->get_dir_path().isEmpty())
    {
        delete vm;
        return false;
    }

    foreach(VMConfig *vm_exist, virtual_machines)
    {
        if (vm_exist->get_name() == vm->get_name() || vm_exist->get_dir_path() == vm->get_dir_path())
        {
            delete vm;
            return false;
        }
    }
    virtual_machines.append(vm);
    save_config_file();
    
    return true;
}

void GlobalConfig::delete_exclude_vm(const QString &del_vm_name, bool delete_vm)
{
    foreach(VMConfig *vm, virtual_machines)
    {
        if (vm->get_name() == del_vm_name)
        {
            if (delete_vm)
                vm->remove_directory_vm();
            virtual_machines.removeOne(vm);
            delete vm;
            break;
        }
    }
    save_config_file();
}

void GlobalConfig::vm_is_created(VMConfig *vm_config)
{
    if (vm_config->save_vm_config(vm_config->get_dir_path()))
    {
        vm_config->setParent(this);
        virtual_machines.append(vm_config);

        save_config_file();

        emit globalConfig_new_vm_is_complete();
    }
}


