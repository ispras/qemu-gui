#include "VMConfig.h"
#include "DeviceStorage.h"
#include "DeviceSystem.h"
#include "DeviceUsb.h"

const QString const_xml_name = "vm.xml";
const QString xml_parameters = "VMParameters";
const QString xml_field_name = "Name";
const QString xml_field_dir = "Directory_path";
const QString xml_field_img = "Image_path";
const QString xml_platform = "Platform";
const QString xml_cmdLine = "AdditionCommandLine";
const QString xml_kernel = "Kernel";
const QString xml_initrd = "InitialRamDisk";

static bool remove_directory(QDir dir);


VMConfig::VMConfig(QObject *parent, const QString &path_vm)
    : QObject(parent), system("System")
{
    list_of_vm_file = NULL;
    system.setRemovable(false);
    path = path_vm;
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

    readVMConfig();
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

void VMConfig::readVMConfig()
{
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
            else if (xmlReader.name() == xml_platform)
            {
                platform = xmlReader.readElementText();
            }
            else if (xmlReader.name() == xml_cmdLine)
            {
                addCmdLine = xmlReader.readElementText();
            }
            else if (xmlReader.name() == xml_kernel)
            {
                kernel = xmlReader.readElementText();
            }
            else if (xmlReader.name() == xml_initrd)
            {
                initrd = xmlReader.readElementText();
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

        xmlWriter.writeStartElement(xml_platform);
        xmlWriter.writeCharacters(platform);
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement(xml_cmdLine);
        xmlWriter.writeCharacters(addCmdLine);
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement(xml_kernel);
        xmlWriter.writeCharacters(kernel);
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement(xml_initrd);
        xmlWriter.writeCharacters(initrd);
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

void VMConfig::setCmdLine(const QString &cmdLine)
{
    addCmdLine = cmdLine;
}

void VMConfig::addDefaultBus(const QString &image)
{
    Device *pci = new DevicePciController(&system);
    pci->setRemovable(false);
    Device *ide = new DeviceIdeController(pci);
    ide->setRemovable(false);
    if (!image.isEmpty())
    {
        new DeviceIdeHd(image, ide->getDevices().at(0));
    }
}

void VMConfig::addDeviceMemory(const QString &size)
{
    (new DeviceMemory(size, &system))->setRemovable(false);
}

void VMConfig::addDeviceMachine(const QString &name)
{
    (new DeviceMachine(name, &system))->setRemovable(false);
}

void VMConfig::addDeviceCpu(const QString &name)
{
    (new DeviceCpu(name, &system))->setRemovable(false);
}

void VMConfig::setKernel(const QString &name)
{
    kernel = name;
}

void VMConfig::setInitrd(const QString &name)
{
    initrd = name;
}

void VMConfig::addUsbDevice()
{
    qDebug() << "---usb" << (new DeviceUsb(&system))->getDeviceTypeName();
    //save_vm_config();
}

QString VMConfig::get_vm_info()
{
    QString info = "Name: " + name_vm + "\n" + "Directory: " + dir_path + "\n" + 
        "Platform: " + platform + "\n";
    QString kernelInfo = (!kernel.isEmpty()) ? "Kernel: " + kernel + "\n" : "";
    QString initrdInfo = (!initrd.isEmpty()) ? "Initial ram disk: " + initrd + "\n" : "";
    info += (kernelInfo + initrdInfo);
    info += system.getCommonDeviceInfo();
    return info;
}

void VMConfig::setPlatform(const QString &platformVM)
{
    platform = platformVM;
}

QString VMConfig::getPlatform()
{
    return platform;
}

QString VMConfig::getMachine()
{
    foreach(Device *dev, system.getDevices())
    {
        if (dev->getDeviceTypeName() == "DeviceMachine")
        {
            DeviceMachine *machine = dynamic_cast<DeviceMachine *>(dev);
            return machine->getName();
        }
    }
    return "";
}

QString VMConfig::get_name()
{
    return name_vm;
}

QString VMConfig::getKernel()
{
    return kernel;
}

QString VMConfig::getInitrd()
{
    return initrd;
}

QString VMConfig::getCmdLine()
{
    return addCmdLine;
}

QString VMConfig::get_dir_path()
{
    return dir_path;
}

QString VMConfig::getCommandLine(CommandLineParameters &cmdParams)
{
    return QString(" -net none")
        + (kernel.isEmpty() ? "" : " -kernel " + kernel)
        + (initrd.isEmpty() ? "" : " -initrd " + initrd)
        + system.getCommandLine(cmdParams) + " " + addCmdLine;
}

QString VMConfig::getPathRRDir()
{
    return get_dir_path() + "/RecordReplay";
}

void VMConfig::remove_directory_vm()
{
    remove_directory_vm(dir_path);
}

void VMConfig::remove_directory_vm(const QString & dir)
{
    QDir del_dir(dir);
    if (del_dir.exists())
        remove_directory(del_dir);
}

void VMConfig::fillReplayList()
{
    QDir rrDir(getPathRRDir());
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

