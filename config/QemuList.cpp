#include "QemuList.h"
#include "GlobalConfig.h"

const QString configName = "Qemu.conf";
const QString xml_qemu_intallation = "QEMUInstallation";
const QString xml_qemu_installation_path = "Path";

QemuList::QemuList()
{
    loadConfig();
}

QemuList &QemuList::instance()
{
    static QemuList inst;
    return inst;
}

void QemuList::addQemuInstallation(const QString &path)
{
    instance().qemuList.append(path);
    instance().saveConfig();
}

void QemuList::delQemuInstallation(const QString &path)
{
    instance().qemuList.removeOne(path);
    instance().saveConfig();
}

const QStringList &QemuList::getAllQemuInstallations()
{
    return instance().qemuList;
}

void QemuList::loadConfig()
{
    QFile file(GlobalConfig::get_home_dir() + "/" + configName);
    if (file.open(QIODevice::ReadOnly))
    {
        QXmlStreamReader xmlReader(&file);
        xmlReader.readNextStartElement();

        while (!xmlReader.atEnd())
        {
            if (xmlReader.name() == xml_qemu_intallation)
            {
                xmlReader.readNextStartElement();
                if (xmlReader.name() == xml_qemu_installation_path)
                {
                    QString qemu_list_item = xmlReader.readElementText();
                    qemuList.append(qemu_list_item);
                    xmlReader.readNextStartElement();
                }
            }
            xmlReader.readNext();
        }
    }
}

void QemuList::saveConfig()
{
    QFile file(GlobalConfig::get_home_dir() + "/" + configName);
    if (file.open(QIODevice::WriteOnly))
    {
        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("QemuList");

        foreach(QString dir, qemuList)
        {
            xmlWriter.writeStartElement(xml_qemu_intallation);
            xmlWriter.writeStartElement(xml_qemu_installation_path);
            xmlWriter.writeCharacters(dir);
            xmlWriter.writeEndElement();
            xmlWriter.writeEndElement();
        }
        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
    }
}
