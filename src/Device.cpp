#include "Device.h"

Device::Device(const QString &n, QObject *parent)
    : QObject(parent), name(n)
{

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
