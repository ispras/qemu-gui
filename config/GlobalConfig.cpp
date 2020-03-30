#include <QColor>
#include "GlobalConfig.h"
#include "QemuList.h"

const QString xml_vm_directory = "VMDirectory";
const QString xml_vm_directory_item = "Dir";
const QString xml_terminal_settings = "TerminalSettings";
const QString xml_terminal_backgroud = "BackgroundColor";
const QString xml_terminal_text_color = "TextColor";
const QString xml_terminal_font_family = "FontFamily";
const QString xml_terminal_font_size = "FontSize";
const QString xml_qmp_port = "QMPPort";
const QString xml_monitor_port = "MonitorPort";
const QString xml_qemu_installation = "QemuInstallation";

// default values
const QString qmp_port_default = "2323";
const QString monitor_port_default = "2424";
const QString bckgrnd_color_default = "#000000";
const QString text_color_default = "#00ff00";
const QString font_family_default = "Courier New";
const QString font_size_default = "12";


GlobalConfig::GlobalConfig()
{
    port_qmp = qmp_port_default;
    port_monitor = monitor_port_default;
    terminal_parameters_background = bckgrnd_color_default;
    terminal_parameters_text_color = text_color_default;
    terminal_parameters_font_family = font_family_default;
    terminal_parameters_font_size = font_size_default;

    QDir home_dir(get_home_dir());
    if (!home_dir.exists())
    {
        home_dir.mkdir(get_home_dir());
    }
    vm_config_file = new QFile(get_home_dir() + "/VMconfig.conf");
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
                            VMConfig *vm = new VMConfig(path);
                            virtual_machines.append(vm);
                            xmlReader.readNextStartElement();
                        }
                    }
                    if (xmlReader.name() == xml_terminal_settings)
                    {
                        xmlReader.readNextStartElement();
                        if (xmlReader.name() == xml_terminal_backgroud)
                        {
                            terminal_parameters_background = xmlReader.readElementText();
                            xmlReader.readNextStartElement();
                        }
                        if (xmlReader.name() == xml_terminal_text_color)
                        {
                            terminal_parameters_text_color = xmlReader.readElementText();
                            xmlReader.readNextStartElement();
                        }
                        if (xmlReader.name() == xml_terminal_font_family)
                        {
                            terminal_parameters_font_family = xmlReader.readElementText();
                            xmlReader.readNextStartElement();
                        }
                        if (xmlReader.name() == xml_terminal_font_size)
                        {
                            terminal_parameters_font_size = xmlReader.readElementText();
                            xmlReader.readNextStartElement();
                        }
                    }
                    if (xmlReader.name() == xml_qemu_installation)
                    {
                        current_qemu = xmlReader.readElementText();
                    }
                    if (xmlReader.name() == xml_qmp_port)
                    {
                        port_qmp = xmlReader.readElementText();
                    }
                    if (xmlReader.name() == xml_monitor_port)
                    {
                        port_monitor = xmlReader.readElementText();
                    }
                }
                xmlReader.readNext();
            }
            vm_config_file->close();
        }
    }
}

GlobalConfig &GlobalConfig::instance()
{
    static GlobalConfig inst;
    return inst;
}

QString GlobalConfig::get_home_dir()
{
    return QDir::homePath() + "/QemuGUI_VirtualMachines";
}

QList<VMConfig *> GlobalConfig::get_exist_vm()
{
    return instance().virtual_machines;
}

void GlobalConfig::set_current_qemu(const QString & qemu)
{
    instance().current_qemu = qemu;
    instance().save_config_file();
}

QString GlobalConfig::get_current_qemu()
{
    return instance().current_qemu;
}

QString GlobalConfig::get_current_qemu_dir()
{
    return QemuList::getQemuDir(get_current_qemu());
}

void GlobalConfig::set_terminal_parameters(QColor background, QColor text_color, const QString & font_family, int font_size)
{
    instance().terminal_parameters_background = background.name();
    instance().terminal_parameters_text_color = text_color.name();
    instance().terminal_parameters_font_family = font_family;
    instance().terminal_parameters_font_size = QString::number(font_size);
    instance().save_config_file();
}

QColor GlobalConfig::get_terminal_backgroud()
{
    return QColor(instance().terminal_parameters_background);
}

QColor GlobalConfig::get_terminal_text_color()
{
    return QColor(instance().terminal_parameters_text_color);
}

QString GlobalConfig::get_terminal_font_family()
{
    return instance().terminal_parameters_font_family;
}

int GlobalConfig::get_terminal_font_size()
{
    return instance().terminal_parameters_font_size.toInt();
}

QString GlobalConfig::get_port_qmp()
{
    return instance().port_qmp;
}

QString GlobalConfig::get_port_monitor()
{
    return instance().port_monitor;
}

void GlobalConfig::set_port_qmp(const QString &port)
{
    instance().port_qmp = port;
    instance().save_config_file();
}

void GlobalConfig::set_port_monitor(const QString &port)
{
    instance().port_monitor = port;
    instance().save_config_file();
}

VMConfig * GlobalConfig::get_vm_by_name(const QString &name)
{
    foreach(VMConfig *vm, instance().virtual_machines)
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

        xmlWriter.writeStartElement(xml_terminal_settings);
        {
            xmlWriter.writeStartElement(xml_terminal_backgroud);
            xmlWriter.writeCharacters(terminal_parameters_background);
            xmlWriter.writeEndElement();

            xmlWriter.writeStartElement(xml_terminal_text_color);
            xmlWriter.writeCharacters(terminal_parameters_text_color);
            xmlWriter.writeEndElement();

            xmlWriter.writeStartElement(xml_terminal_font_family);
            xmlWriter.writeCharacters(terminal_parameters_font_family);
            xmlWriter.writeEndElement();

            xmlWriter.writeStartElement(xml_terminal_font_size);
            xmlWriter.writeCharacters(terminal_parameters_font_size);
            xmlWriter.writeEndElement();
        }
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement(xml_qemu_installation);
        xmlWriter.writeCharacters(current_qemu);
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement(xml_qmp_port);
        xmlWriter.writeCharacters(port_qmp);
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement(xml_monitor_port);
        xmlWriter.writeCharacters(port_monitor);
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
    VMConfig *vm = new VMConfig(path);

    if (vm->get_name().isEmpty() || vm->get_dir_path().isEmpty())
    {
        delete vm;
        return false;
    }

    foreach(VMConfig *vm_exist, instance().virtual_machines)
    {
        if (vm_exist->get_name() == vm->get_name() || vm_exist->get_dir_path() == vm->get_dir_path())
        {
            delete vm;
            return false;
        }
    }
    instance().virtual_machines.append(vm);
    instance().save_config_file();
    
    return true;
}

void GlobalConfig::delete_exclude_vm(const QString &del_vm_name, bool delete_vm)
{
    foreach(VMConfig *vm, instance().virtual_machines)
    {
        if (vm->get_name() == del_vm_name)
        {
            if (delete_vm)
                vm->remove_directory_vm();
            instance().virtual_machines.removeOne(vm);
            delete vm;
            break;
        }
    }
    instance().save_config_file();
}

void GlobalConfig::vm_is_created(VMConfig *vm_config)
{
    if (vm_config->save_vm_config(vm_config->get_dir_path()))
    {
        instance().virtual_machines.append(vm_config);

        instance().save_config_file();
    }
}
