#include "Device.h"
#include "DeviceFactory.h"

/*

Device properties:
 - parent bus type
 - child buses
   - type of children
   - number of children
*/

Device::Device(const QString &n, Device *parent)
    : QObject(NULL), name(n)
{
    if (parent)
        parent->addDevice(this);
}

void Device::addDevice(Device *dev)
{
    devices.append(dev);
    dev->setParent(this);
}

QString Device::getDescription() const
{
    return name;
}

void Device::save(QXmlStreamWriter &xml) const
{
    xml.writeStartElement(getDeviceTypeName());

    xml.writeStartElement("Name");
    xml.writeCharacters(name);
    xml.writeEndElement();

    foreach(Device *dev, devices)
        dev->save(xml);
    
    xml.writeEndElement();
}

void Device::read(QXmlStreamReader &xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == getDeviceTypeName());

    xml.readNextStartElement();
    Q_ASSERT(xml.name() == "Name");
    name = xml.readElementText();

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
