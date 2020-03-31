#include "QemuList.h"
#include "GlobalConfig.h"
#include "common/FileHelpers.h"
#include "PlatformInformationReader.h"

const QString configName = "Qemu.conf";
const QString platformPrefix = "/platforms/qemu_";
const QString xml_qemu_intallation = "QEMUInstallation";
const QString xml_qemu_installation_name = "Name";
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

QString QemuList::getQemuProfilePath(const QString &name)
{
    return GlobalConfig::get_home_dir() + platformPrefix + name;
}

void QemuList::addQemuInstallation(const QString &name, const QString &path)
{
    new PlatformInformationReader(name);

    instance().qemuList.insert(name, path);
    instance().saveConfig();
}

void QemuList::delQemuInstallation(const QString &name)
{
    instance().qemuList.remove(name);
    instance().saveConfig();
    FileHelpers::deleteDirectory(GlobalConfig::get_home_dir() + platformPrefix + name);
}

QString QemuList::getQemuDir(const QString &name)
{
    return instance().qemuList.value(name);
}

QString QemuList::getQemuExecutablePath(const QString &qemu, const QString &platform)
{
    return getQemuDir(qemu)
#ifdef Q_OS_WIN
        + "/" + "qemu-system-" + platform + "w.exe";
#else
        + "/" + "qemu-system-" + platform;
#endif
}

const QemuList::Items &QemuList::getAllQemuInstallations()
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
                QString name, path;
                if (xmlReader.name() == xml_qemu_installation_name)
                {
                    name = xmlReader.readElementText();
                    xmlReader.readNextStartElement();
                }
                if (xmlReader.name() == xml_qemu_installation_path)
                {
                    path = xmlReader.readElementText();
                    xmlReader.readNextStartElement();
                }
                if (!name.isEmpty() && !path.isEmpty())
                {
                    qemuList.insert(name, path);
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

        for (auto it = qemuList.begin() ; it != qemuList.end() ; ++it)
        {
            xmlWriter.writeStartElement(xml_qemu_intallation);
                xmlWriter.writeStartElement(xml_qemu_installation_name);
                    xmlWriter.writeCharacters(it.key());
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement(xml_qemu_installation_path);
                    xmlWriter.writeCharacters(it.value());
                xmlWriter.writeEndElement();
            xmlWriter.writeEndElement();
        }
        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
    }
}
