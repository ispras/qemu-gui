#include "QemuGUI.h"

#include "GlobalConfig.h"
#include <QtWidgets/QApplication>

static bool remove_directory(QDir dir);

static QString parse_vm_dir(QString path)
{
    QDir home_dir(path);
    QStringList vm_files = home_dir.entryList(QDir::Filter::Files);
    
    foreach (const QString vm_file, vm_files)
    {
        if (vm_file.contains(".xml", Qt::CaseSensitivity::CaseInsensitive))
        {
            QString xml_name = path + "/" + vm_file;

            QFile file(xml_name);
            if (file.open(QIODevice::ReadOnly))
            {
                QXmlStreamReader xmlReader(&file);

                xmlReader.readNext();

                while (!xmlReader.atEnd())
                {
                    if (xmlReader.isStartElement())
                    {
                        if (xmlReader.name() == "Name")
                        {
                            return xmlReader.readElementText();
                        }
                    }
                    xmlReader.readNext();
                }
                file.close();
            }
        }
    }

    return "";
}

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
            names_VM.append(parse_vm_dir(path_to_vm));
            vm_info.name.append(parse_vm_dir(path_to_vm));
            vm_info.path.append(path_to_vm);
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

QStringList GlobalConfig::get_exist_vm()
{
    return vm_info.name;
}

QString GlobalConfig::get_created_vm_name()
{
    return vm_info.name.back();
}

bool GlobalConfig::add_exist_vm(QString path)
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly))
    {
        QString name_vm = "", path_vm = "";
        QXmlStreamReader xmlReader(&file);
        
        xmlReader.readNext();
        while (!xmlReader.atEnd())
        {
            if (xmlReader.isStartElement())
            {
                if (xmlReader.name() == "Name")
                {
                    name_vm = xmlReader.readElementText();
                }
                if (xmlReader.name() == "Directory_path")
                {
                    path_vm = xmlReader.readElementText();
                }
            }
            xmlReader.readNext();
        }
        file.close();
        if (name_vm != "" && path_vm != "")
        {
            if (!vm_info.name.contains(name_vm, Qt::CaseInsensitive) && !vm_info.path.contains(path_vm, Qt::CaseInsensitive))
            {
                vm_info.name.append(name_vm);
                vm_info.path.append(path_vm);

                if (vm_config_file->open(QIODevice::Append))
                {
                    QTextStream stream(vm_config_file);
                    stream << path_vm << endl;
                    vm_config_file->close();
                }
                return true;
            }
        }
        return false;
    }
    return false;
}

void GlobalConfig::delete_vm(QString del_vm_name)
{
    int index_del_name = vm_info.name.indexOf(del_vm_name);
    QString del_path = vm_info.path.at(index_del_name);

    vm_info.name.removeAt(index_del_name);
    vm_info.path.removeAt(index_del_name);

    if (vm_config_file->open(QIODevice::WriteOnly))
    {
        QTextStream stream(vm_config_file);
        foreach(const QString vm_path, vm_info.path)
        {
            stream << vm_path << endl;
        }
        vm_config_file->close();
    }

    QDir del_dir(del_path);
    if (del_dir.exists())
        remove_directory(del_dir);
}

void GlobalConfig::exclude_vm(QString del_vm_name)
{
    int index_del_name = vm_info.name.indexOf(del_vm_name);
    vm_info.name.removeAt(index_del_name);
    vm_info.path.removeAt(index_del_name);

    if (vm_config_file->open(QIODevice::WriteOnly))
    {
        QTextStream stream(vm_config_file);
        foreach(const QString vm_path, vm_info.path)
        {
            stream << vm_path << endl;
        }
        vm_config_file->close();
    }
}

void GlobalConfig::vm_is_created(VMConfig *vm_config)
{
    QString new_vm_path;
    if (vm_config->vm_config_get_dir_path() != "")
    {
        new_vm_path = vm_config->vm_config_get_dir_path();
    }
    else
    {
        new_vm_path = path_to_home_dir + "/" + vm_config->vm_config_get_name();
    }
    QDir new_vm_dir(new_vm_path);
    if (!new_vm_dir.exists())
    {
        QDir().mkdir(new_vm_path);
    }
    
    if (vm_config_file->open(QIODevice::Append))
    {
        if (vm_config->vm_config_make(new_vm_path))
        {
            vm_config->setParent(this);

            names_VM.append(new_vm_path);
            vm_info.name.append(parse_vm_dir(new_vm_path));
            vm_info.path.append(new_vm_path);
            QTextStream stream(vm_config_file);
            stream << new_vm_path << endl;
            vm_config_file->close();
            emit globalConfig_new_vm_is_complete();
        }
    }
    else
    {
        QMessageBox::critical(nullptr, "Error", "Config file doesn't open");
    }
}

static bool remove_directory(QDir dir)
{
    int res = 0;
    QStringList dirs = dir.entryList(QDir::Dirs | QDir::AllDirs | QDir::NoDotAndDotDot);
    QStringList files = dir.entryList(QDir::Files);

    foreach(const QString f_name, files)
    {
        QString del_file = dir.absolutePath() + "/" + f_name;
        if (QFile::remove(del_file))
            qDebug() << "deleting file ok!";
    }

    foreach(QString d_name, dirs)
    {
        remove_directory(QDir(dir.absolutePath() + "/" + d_name));
    }

    if (!QDir().rmdir(dir.absolutePath()))
    {
        res = false;
    }
    return res;
}


