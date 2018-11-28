#include "DeviceStorage.h"
#include "DeviceBus.h"
#include "DeviceFactory.h"
#include "CommandLineParameters.h"
#include "DeviceForm.h"

DeviceStorageController::DeviceStorageController(const QString &n, Device *parent)
    : Device(n, parent)
{

}

const char DeviceIdeController::typeName[] = "DeviceIdeController";
REGISTER_DEVICE(DeviceIdeController)

DeviceIdeController::DeviceIdeController(Device *parent)
    : DeviceStorageController("IDE", parent)
{
    initDefault();
}

void DeviceIdeController::initDefault()
{
    new DeviceBusIde(0, this);
    new DeviceBusIde(1, this);
    // TODO: allow non-default ide controllers
    setId("ide");
}

DeviceStorage::DeviceStorage(const QString &n, Device *parent)
    : Device(n, parent)
{

}

/******************************************************************************
 * IDE HDD                                                                    *
 ******************************************************************************/

const char DeviceIdeHd::typeName[] = "DeviceIdeHd";
REGISTER_DEVICE(DeviceIdeHd)

static const char xml_image[] = "Image";

DeviceIdeHd::DeviceIdeHd()
    : DeviceStorage("IDE-HD", NULL)
{
}

DeviceIdeHd::DeviceIdeHd(const QString &img, Device *parent)
    : DeviceStorage("IDE-HD", parent), image(img)
{
}

QWidget *DeviceIdeHd::getEditorForm()
{
    return new DeviceIdeHdForm(this);
}

void DeviceIdeHd::saveParameters(QXmlStreamWriter &xml) const
{
    xml.writeStartElement(xml_image);
    xml.writeCharacters(image);
    xml.writeEndElement();
}

void DeviceIdeHd::readParameters(QXmlStreamReader &xml)
{
    xml.readNextStartElement();
    Q_ASSERT(xml.name() == xml_image);
    image = xml.readElementText();
}

QString DeviceIdeHd::getCommandLineOption(CommandLineParameters &cmdParams)
{
    DeviceBusIde *bus = dynamic_cast<DeviceBusIde*>(parent());
    Q_ASSERT(bus);
    DeviceIdeController *ide = dynamic_cast<DeviceIdeController*>(bus->parent());
    Q_ASSERT(ide);

    if (cmdParams.getLaunchMode() == LaunchMode::NORMAL)
    {
        QString cmdFile = " -drive file=" + image + ",if=none,id="
            + getId() + "-file";
        return  cmdFile + " -device ide-hd"
            +",bus=" + ide->getId() + "." + QString::number(bus->getNumber()) 
            + ",drive=" + getId() + "-file"
            + ",id=" + getId();
    }
    else
    {
        QString overlay = cmdParams.getOverlayForImage(image);
        QString cmdFile = "-drive file=" + overlay + ",if=none,id="
             + getId() + "-file";

        return cmdFile + " -drive driver=blkreplay,if=none,image="
            + getId() + "-file,id=" + getId() 
            + "-driver -device ide-hd,drive=" + getId() + "-driver"
            +",bus=" + ide->getId() + "." + QString::number(bus->getNumber()) 
            + ",id=" + getId();
    }
}

bool DeviceIdeHd::isDeviceValid()
{
    return !getImage().isEmpty();
}


/******************************************************************************
* CDROM                                                                       *
******************************************************************************/

const char DeviceIdeCdrom::typeName[] = "DeviceIdeCdrom";
REGISTER_DEVICE(DeviceIdeCdrom)

DeviceIdeCdrom::DeviceIdeCdrom()
    : DeviceStorage("IDE-CDROM", NULL)
{
}

DeviceIdeCdrom::DeviceIdeCdrom(const QString &img, Device *parent)
    : DeviceStorage("IDE-CDROM", parent), image(img)
{
}

QWidget *DeviceIdeCdrom::getEditorForm()
{
    return new DeviceIdeCdromForm(this);
}

void DeviceIdeCdrom::saveParameters(QXmlStreamWriter &xml) const
{
    xml.writeStartElement(xml_image);
    xml.writeCharacters(image);
    xml.writeEndElement();
}

void DeviceIdeCdrom::readParameters(QXmlStreamReader &xml)
{
    xml.readNextStartElement();
    Q_ASSERT(xml.name() == xml_image);
    image = xml.readElementText();
}

QString DeviceIdeCdrom::getCommandLineOption(CommandLineParameters &cmdParams)
{
    DeviceBusIde *bus = dynamic_cast<DeviceBusIde*>(parent());
    Q_ASSERT(bus);
    DeviceIdeController *ide = dynamic_cast<DeviceIdeController*>(bus->parent());
    Q_ASSERT(ide);

    if (cmdParams.getLaunchMode() == LaunchMode::NORMAL)
    {
        QString cmdFile = " -drive file=" + image + ",if=none,id="
            + getId() + "-file";
        return  cmdFile + " -device ide-cd"
            + ",bus=" + ide->getId() + "." + QString::number(bus->getNumber())
            + ",drive=" + getId() + "-file"
            + ",id=" + getId();
    }
    else
    {
        QString overlay = cmdParams.getOverlayForImage(image);
        QString cmdFile = "-drive file=" + overlay + ",if=none,id="
            + getId() + "-file";

        return cmdFile + " -drive driver=blkreplay,if=none,image="
            + getId() + "-file,id=" + getId()
            + "-driver -device ide-cd,drive=" + getId() + "-driver"
            + ",bus=" + ide->getId() + "." + QString::number(bus->getNumber())
            + ",id=" + getId();
    }
}

bool DeviceIdeCdrom::isDeviceValid()
{
    return !getImage().isEmpty();
}


