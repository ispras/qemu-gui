#include "Device.h"

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
    xml.writeStartElement("Device");

    xml.writeStartElement("Name");
    xml.writeCharacters(name);
    xml.writeEndElement();

    foreach(Device *dev, devices)
        dev->save(xml);
    
    xml.writeEndElement();
}
