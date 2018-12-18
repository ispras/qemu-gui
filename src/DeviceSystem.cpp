#include "DeviceSystem.h"
#include "DeviceFactory.h"
#include "DeviceForm.h"

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

QString DeviceMemory::getCommandLineOption()
{
    return "-m " + size + "M";
}



