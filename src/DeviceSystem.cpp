#include "DeviceSystem.h"

const char DeviceSystem::typeName[] = "DeviceSystem";

DeviceSystem::DeviceSystem(const QString &n, Device *parent) :
    Device(n, parent)
{
}

/******************************************************************************
* MEMORY                                                                   *
******************************************************************************/

const char DeviceMemory::typeName[] = "DeviceMemory";
static const char xml_size[] = "MemorySize";


DeviceMemory::DeviceMemory(const QString &memSize, Device *parent) :
    DeviceSystem("Memory", parent), size(memSize)
{}


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

/******************************************************************************
* IDE HDD                                                                    *
******************************************************************************/

DeviceMemoryForm::DeviceMemoryForm(DeviceMemory *dev) : device(dev)
{
    QGroupBox *memoryFormGroup = this;
    QLabel *memLbl = new QLabel("Memory size (MB)");
    QSpinBox *sizeSpin = new QSpinBox(memoryFormGroup);

    sizeSpin->setMaximum(MIN_RAM_VALUE);
    sizeSpin->setMaximum(MAX_RAM_VALUE);
    sizeSpin->setValue(dev->getSize());

    QVBoxLayout *mainLay = new QVBoxLayout();
    QHBoxLayout *topLay = new QHBoxLayout();
    topLay->addWidget(memLbl);
    topLay->addWidget(sizeSpin);

    mainLay->addLayout(topLay);
    mainLay->addStretch(500);

    memoryFormGroup->setLayout(mainLay);
    connect(sizeSpin, SIGNAL(valueChanged(int)), this, SLOT(sizeChanged(int)));
}

void DeviceMemoryForm::sizeChanged(int val)
{
    device->setSize(QString::number(val));
}

