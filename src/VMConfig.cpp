#include "VMConfig.h"
#include "CreateVMForm.h"


const QString const_xml_name = "vm.xml";
const QString xml_field_name = "Name";
const QString xml_field_dir = "Directory_path";
const QString xml_field_img = "Image_path";

static bool remove_directory(QDir dir);


VMConfig::VMConfig(QObject *parent, const QString &path_vm)
    : QObject(parent)
{
    QString path = path_vm;
    QString xml_name;
    //if (path.right(const_xml_name.length()) != const_xml_name)
    if (path_vm.section('/', -1) != const_xml_name)
    {
        path = path + "/" + const_xml_name;
    }

    QFile file(path);
    if (file.open(QIODevice::ReadOnly))
    {
        QXmlStreamReader xmlReader(&file);

        xmlReader.readNext();

        while (!xmlReader.atEnd())
        {
            if (xmlReader.isStartElement())
            {
                if (xmlReader.name() == xml_field_name)
                {
                    name_vm = xmlReader.readElementText();
                }
                if (xmlReader.name() == xml_field_dir)
                {
                    dir_path = xmlReader.readElementText();
                }
                if (xmlReader.name() == xml_field_img)
                {
                    image_path = xmlReader.readElementText();
                }
            }
            xmlReader.readNext();
        }
        file.close();
    }
}

VMConfig::~VMConfig()
{

}


bool VMConfig::save_vm_config(const QString &path)
{
    QDir vm_dir(path);
    if (!vm_dir.exists())
    {
        QDir().mkdir(path);
    }

    QString xml_name;
    xml_name = path + "/" + const_xml_name;

    QFile file(xml_name);
    if (file.open(QIODevice::WriteOnly))
    {
        QXmlStreamWriter xmlWriter(&file);

        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("VMParameters");

        xmlWriter.writeStartElement(xml_field_name);
        xmlWriter.writeCharacters(name_vm);
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement(xml_field_dir);
        if (dir_path != "")
            xmlWriter.writeCharacters(dir_path);
        else
            xmlWriter.writeCharacters(path);
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement(xml_field_img);
        xmlWriter.writeCharacters(image_path);
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
        file.close();
        return true;
    }
    return false;
}

void VMConfig::set_name(const QString &name_vm_)
{
    name_vm = name_vm_;
}

void VMConfig::set_dir_path(const QString &dir_path_)
{
    dir_path = dir_path_;
}

void VMConfig::add_image_path(const QString &image_path_)
{
    image_path = image_path_;
}

QString VMConfig::get_vm_info()
{
    QString info = "Name: " + name_vm + "\n" + "Directory: " + dir_path + "\n" + "Image: " + image_path;
    return info;
}

bool VMConfig::start_qemu(QString qemu_exe)
{
    QString cmd = qemu_exe;
    QProcess qemu;

    qemu.start(qemu_exe);
    qemu.waitForFinished();
    return false;
}

QString VMConfig::get_name()
{
    return name_vm;
}

QString VMConfig::get_dir_path()
{
    return dir_path;
}

QString VMConfig::get_image_path()
{
    return image_path;
}

void VMConfig::remove_directory_vm()
{
    QDir del_dir(dir_path);
    if (del_dir.exists())
        remove_directory(del_dir);
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



