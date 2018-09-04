#include "VMConfig.h"
#include "CreateVMForm.h"
#include "DeviceStorage.h"
#include "DeviceSystem.h"

const QString const_xml_name = "vm.xml";
const QString xml_parameters = "VMParameters";
const QString xml_field_name = "Name";
const QString xml_field_dir = "Directory_path";
const QString xml_field_img = "Image_path";

static bool remove_directory(QDir dir);


VMConfig::VMConfig(QObject *parent, const QString &path_vm)
    : QObject(parent), system("System")
{
    QString path = path_vm;
    QString xml_name;
    if (path_vm.section('/', -1) != const_xml_name)
    {
        dir_path = path;
        path = path + "/" + const_xml_name;
    }
    else
    {
        dir_path = path;
        dir_path.chop(const_xml_name.size());
    }

    QFile file(path);
    if (file.open(QIODevice::ReadOnly))
    {
        QXmlStreamReader xmlReader(&file);

        xmlReader.readNextStartElement();
        Q_ASSERT(xmlReader.name() == xml_parameters);

        while (xmlReader.readNextStartElement())
        {
            if (xmlReader.name() == xml_field_name)
            {
                name_vm = xmlReader.readElementText();
            }
            else if (xmlReader.name() == xml_field_dir)
            {
                //dir_path = xmlReader.readElementText();
            }
            else if (xmlReader.name() == xml_field_img)
            {
                //image_path = xmlReader.readElementText();
            }
            else /* Device */
            {
                system.read(xmlReader);
            }
        }
    }
    else
    {
        /* Default config */
        new Device("CPU", &system);
        new Device("Machine", &system);
    }
}

VMConfig::~VMConfig()
{

}

void VMConfig::createVMFolder(const QString &path) const
{
    QDir vm_dir(path);
    if (!vm_dir.exists())
    {
        QDir().mkdir(path);
    }
}

bool VMConfig::save_vm_config(const QString &path) const
{
    createVMFolder(path);

    QString xml_name;
    xml_name = path + "/" + const_xml_name;

    QFile file(xml_name);
    if (file.open(QIODevice::WriteOnly))
    {
        QXmlStreamWriter xmlWriter(&file);

        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement(xml_parameters);

        xmlWriter.writeStartElement(xml_field_name);
        xmlWriter.writeCharacters(name_vm);
        xmlWriter.writeEndElement();

        system.save(xmlWriter);

        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
        file.close();
        return true;
    }
    return false;
}

void VMConfig::save_vm_config() const
{
    save_vm_config(dir_path);
}

void VMConfig::set_name(const QString &name_vm_)
{
    name_vm = name_vm_;
}

void VMConfig::addDefaultIDE(const QString &image)
{
    Device *ide = new DeviceIdeController(&system);
    new DeviceIdeHd(image, ide->getDevices().at(0));
}

void VMConfig::addMemorySize(const QString &size)
{
    new DeviceMemory(size, &system);
}

void VMConfig::addUsbDevice()
{
    qDebug() << "---usb" << (new DeviceUsb(&system))->getDeviceTypeName();
    //save_vm_config();
}

QString VMConfig::get_vm_info()
{
    QString info = "Name: " + name_vm + "\n" + "Directory: " + dir_path + "\n";
    // TODO: output hw config
    return info;
}

QString VMConfig::get_name()
{
    return name_vm;
}

QString VMConfig::get_dir_path()
{
    return dir_path;
}

QString VMConfig::getCommandLine(CommandLineParameters &cmdParams)
{
    return system.getCommandLine(cmdParams);
}

void VMConfig::remove_directory_vm()
{
    QDir del_dir(dir_path);
    if (del_dir.exists())
        remove_directory(del_dir);
}

void VMConfig::fillReplayList()
{
    QDir rrDir(dir_path + "/RecordReplay");
    QStringList dirs = rrDir.entryList(QDir::Dirs | QDir::AllDirs | QDir::Filter::NoDotAndDotDot);
    replayList.clear();
    foreach (QString name, dirs)
    {
        replayList.append(name);
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
        if (!QFile::remove(del_file))
            qDebug() << "File" << del_file << "was not deleted!";
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

