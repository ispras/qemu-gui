#include "VMConfig.h"
#include "CreateVMForm.h"


VMConfig::VMConfig(QObject *parent)
    : QObject(parent)
{

}

VMConfig::~VMConfig()
{

}


bool VMConfig::vm_config_make(QString path)
{
    QString xml_name;
    xml_name = path + "/" + name_vm + ".xml";

    QMessageBox::about(nullptr, "heh", xml_name);
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

        xmlWriter.writeStartElement("RAM_size");
        xmlWriter.writeCharacters(string_for_digit_params.setNum(ram_size, 10));
        xmlWriter.writeEndElement();

        if (hdd_type == NEW_HDD || hdd_type == EXIST_HDD)
        {
            xmlWriter.writeStartElement("Image_path");
            xmlWriter.writeCharacters(image_path);
            xmlWriter.writeEndElement();
        }

        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
        file.close();
        return true;
    }
    return false;
}

void VMConfig::vm_config_set_name(QString name_vm_)
{
    name_vm = name_vm_;
}

void VMConfig::vm_config_set_dir_path(QString dir_path_)
{
    dir_path = dir_path_;
}

void VMConfig::vm_config_set_ram_size(int ram_size_)
{
    ram_size = ram_size_;
}

void VMConfig::vm_config_set_image_path(QString image_path_)
{
    image_path = image_path_;
}

void VMConfig::vm_config_set_hdd(int hdd)
{
    hdd_type = hdd;
}


QString VMConfig::vm_config_get_name()
{
    return name_vm;
}

QString VMConfig::vm_config_get_dir_path()
{
    return dir_path;
}



