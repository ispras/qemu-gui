#include "QemuGUI.h"

#include "GlobalConfig.h"
#include <QtWidgets/QApplication>

const QString xml_vm_directory = "VMDirectory";
const QString xml_vm_directory_item = "Dir";
const QString xml_qemu_intallation = "QEMUInstallation";
const QString xml_qemu_installation_item = "Install_path";


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
        if (vm_config_file->open(QIODevice::ReadOnly))
        {
            QXmlStreamReader xmlReader(vm_config_file);

            xmlReader.readNext();

            while (!xmlReader.atEnd())
            {
                if (xmlReader.isStartElement())
                {
                    if (xmlReader.name() == xml_vm_directory)
                    {
                        xmlReader.readNextStartElement();
                        while (xmlReader.name() == xml_vm_directory_item)
                        {
                            QString path = xmlReader.readElementText();
                            VMConfig *vm = new VMConfig(this, path);
                            virtual_machines.append(vm);
                            xmlReader.readNextStartElement();
                        }
                    }
                    if (xmlReader.name() == xml_qemu_intallation)
                    {
                        xmlReader.readNextStartElement();
                        while (xmlReader.name() == xml_qemu_installation_item)
                        {
                            qemu_list.append(xmlReader.readElementText());
                            xmlReader.readNextStartElement();
                        }
                    }
                }
                xmlReader.readNext();
            }
            vm_config_file->close();
        }
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

void GlobalConfig::add_qemu_installation_dir(const QString & qemu_install_path)
{
    qemu_list.append(qemu_install_path);
    save_config_file();
}

void GlobalConfig::del_qemu_installation_dir(const QString & qemu_install_path)
{
    qemu_list.removeOne(qemu_install_path);
    save_config_file();
}

QStringList & GlobalConfig::get_qemu_installation_dirs()
{
    return qemu_list;
}

VMConfig * GlobalConfig::get_vm_by_name(const QString &name)
{
    foreach(VMConfig *vm, virtual_machines)
    {
        if (vm->get_name() == name)
            return vm;
    }
    return nullptr;
}

bool GlobalConfig::save_config_file()
{
    if (vm_config_file->open(QIODevice::WriteOnly))
    {
        QXmlStreamWriter xmlWriter(vm_config_file);

        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("CommonParameters");

        xmlWriter.writeStartElement(xml_vm_directory);
        foreach(VMConfig *vm, virtual_machines)
        {
            xmlWriter.writeStartElement(xml_vm_directory_item);
            xmlWriter.writeCharacters(vm->get_dir_path());
            xmlWriter.writeEndElement();
        }
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement(xml_qemu_intallation);
        foreach(QString dir, qemu_list)
        {
            xmlWriter.writeStartElement(xml_qemu_installation_item);
            xmlWriter.writeCharacters(dir);
            xmlWriter.writeEndElement();
        }
        xmlWriter.writeEndElement();
        
        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
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


