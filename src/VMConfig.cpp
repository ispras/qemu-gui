#include "VMConfig.h"
#include "CreateVMForm.h"


const QString const_xml_name = "vm.xml";

static bool remove_directory(QDir dir);


VMConfig::VMConfig(QObject *parent, const QString &path_vm)
    : QObject(parent)
{
    QString path = path_vm;
    if (path.contains(const_xml_name, Qt::CaseSensitivity::CaseInsensitive))
    {
        path.chop(const_xml_name.length());
    }

    QDir home_dir(path);
    QStringList vm_files = home_dir.entryList(QDir::Filter::Files);

    foreach(const QString vm_file, vm_files)
    {
        if (vm_file == const_xml_name)
        {
            QString xml_name = path + "/" + const_xml_name;

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
                            name_vm = xmlReader.readElementText();
                        }
                        if (xmlReader.name() == "Directory_path")
                        {
                            dir_path = xmlReader.readElementText();
                        }
                        if (xmlReader.name() == "Image_path")
                        {
                            image_path = xmlReader.readElementText();
                        }
                    }
                    xmlReader.readNext();
                }
                file.close();
            }
            break;
        }
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

        QString string_for_digit_params;

        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("VMParameters");

        xmlWriter.writeStartElement("Name");
        xmlWriter.writeCharacters(name_vm);
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("Directory_path");
        if (dir_path != "")
            xmlWriter.writeCharacters(dir_path);
        else
            xmlWriter.writeCharacters(path);
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("Image_path");
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

QString VMConfig::get_name()
{
    return name_vm;
}

QString VMConfig::get_dir_path()
{
    return dir_path;
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



