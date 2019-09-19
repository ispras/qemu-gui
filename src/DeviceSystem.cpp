#include "DeviceSystem.h"
#include "DeviceFactory.h"
#include "DeviceForm.h"
#include "PlatformInfo.h"

/******************************************************************************
* MEMORY                                                                   *
******************************************************************************/

const char DeviceMemory::typeName[] = "DeviceMemory";
REGISTER_DEVICE(DeviceMemory)

static const char xml_size[] = "MemorySize";

DeviceMemory::DeviceMemory(const QString &memSize, Device *parent) :
    Device("Memory", parent), size(memSize)
{
}

QWidget *DeviceMemory::getEditorForm()
{
    return new DeviceMemoryForm(this);
}

void DeviceMemory::saveParameters(QXmlStreamWriter &xml) const
{
    xml.writeStartElement(xml_size);
    xml.writeCharacters(size);
    xml.writeEndElement();
}

void DeviceMemory::readParameters(QXmlStreamReader &xml)
{
    xml.readNextStartElement();
    Q_ASSERT(xml.name() == xml_size);
    size = xml.readElementText();
}

QString DeviceMemory::getCommandLineOption(CommandLineParameters &cmdParams)
{
    return " -m " + size + "M";
}

QString DeviceMemory::getDeviceInfo()
{
    return "Memory: " + size + " Mb\n";
}


/******************************************************************************
* System unchanged devices                                                    *
******************************************************************************/

static const char xml_name[] = "Model";

DeviceSystem::DeviceSystem(const QString &n, Device *parent)
    : Device(n, parent)
{
}

void DeviceSystem::saveParameters(QXmlStreamWriter &xml) const
{
    xml.writeStartElement(xml_name);
    xml.writeCharacters(getName());
    xml.writeEndElement();
}

void DeviceSystem::readParameters(QXmlStreamReader &xml)
{
    xml.readNextStartElement();
    Q_ASSERT(xml.name() == xml_name);
    setName(xml.readElementText());
}

/******************************************************************************
* MACHINE                                                                     *
******************************************************************************/

const char DeviceMachine::typeName[] = "DeviceMachine";
REGISTER_DEVICE(DeviceMachine)

DeviceMachine::DeviceMachine(const QString &machineName, Device *parent) :
    DeviceSystem("Machine", parent)
{
    setName(machineName);
}

QString DeviceMachine::getCommandLineOption(CommandLineParameters &cmdParams)
{
    return " -machine " + getName();
}

QString DeviceMachine::getDeviceInfo()
{
    return "Machine: " + getName() + "\n";
}

/******************************************************************************
* CPU                                                                         *
******************************************************************************/

const char DeviceCpu::typeName[] = "DeviceCpu";
REGISTER_DEVICE(DeviceCpu)

DeviceCpu::DeviceCpu(const QString &cpuName, Device *parent) :
    DeviceSystem("CPU", parent)
{
    setName(cpuName);
}

QString DeviceCpu::getCommandLineOption(CommandLineParameters &cmdParams)
{
    return getName() == "default" ? "" : " -cpu " + getName();
}

QString DeviceCpu::getDeviceInfo()
{
    return "CPU: " + getName() + "\n";
}

QWidget *DeviceCpu::getEditorForm()
{
    return new DeviceCpuForm(this);
}

const QStringList DeviceCpu::getCpuModels() const
{
    PlatformInfo pi(getPathToConfig());
    return pi.getCpus();
}

