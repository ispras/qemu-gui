#include "Device.h"
#include "DeviceFactory.h"

/*

Device properties:
 - parent bus type
 - child buses
   - type of children
   - number of children
*/

static const char xml_name[] = "Name";

Device::Device()
{
    init();
}

Device::Device(const QString &n, Device *parent)
    : QObject(NULL), name(n)
{
    init();
    if (parent)
        parent->addDevice(this);
}

void Device::init()
{
    static int lastId = 0;

    id = "device-" + QString::number(lastId++);
}

void Device::addDevice(Device *dev)
{
    devices.append(dev);
    dev->setParent(this);
}

void Device::removeDevice(Device *dev)
{
    devices.removeOne(dev);
}

QString Device::getDescription() const
{
    return name;
}

void Device::save(QXmlStreamWriter &xml) const
{
    xml.writeStartElement(getDeviceTypeName());

    xml.writeStartElement(xml_name);
    xml.writeCharacters(name);
    xml.writeEndElement();

    saveParameters(xml);

    foreach(Device *dev, devices)
        dev->save(xml);
    
    xml.writeEndElement();
}

void Device::read(QXmlStreamReader &xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == getDeviceTypeName());

    xml.readNextStartElement();
    Q_ASSERT(xml.name() == xml_name);
    name = xml.readElementText();

    readParameters(xml);

    // default children
    foreach(Device *dev, devices)
    {
        xml.readNextStartElement();
        dev->read(xml);
    }

    while (xml.readNextStartElement())
    {
        Device *dev = DeviceFactory::createDevice(xml.name());
        addDevice(dev);
        dev->read(xml);
    }
}

QString Device::getCommandLine(CommandLineParameters &cmdParams)
{
    QString res = getCommandLineOption(cmdParams);
    foreach(Device *dev, devices)
        res += dev->getCommandLine(cmdParams);
    return res;
}
