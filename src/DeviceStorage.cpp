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
const char DeviceIdeController::deviceName[] = "IDE";
static const char xml_removable[] = "Removable";
REGISTER_DEVICE(DeviceIdeController)

DeviceIdeController::DeviceIdeController()
    : DeviceStorageController(deviceName, NULL)
{
    initDefault();
    setRemovable(true);
}

DeviceIdeController::DeviceIdeController(Device *parent)
    : DeviceStorageController(deviceName, parent)
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

const char DevicePciController::typeName[] = "DevicePciController";
REGISTER_DEVICE(DevicePciController)

DevicePciController::DevicePciController(Device *parent)
    : DeviceStorageController("PCI", parent)
{
    initDefault();
}

void DevicePciController::initDefault()
{
    setId("pci");
}

const char DeviceScsiController::typeName[] = "DeviceScsiController";
const char DeviceScsiController::deviceName[] = "SCSI";
REGISTER_DEVICE(DeviceScsiController)

DeviceScsiController::DeviceScsiController()
    : DeviceStorageController(deviceName, NULL)
{
    initDefault();
}

DeviceScsiController::DeviceScsiController(Device *parent)
    : DeviceStorageController(deviceName, parent)
{
    initDefault();
}

void DeviceScsiController::initDefault()
{
    setId("scsi");
    setRemovable(true);
}

QString DeviceScsiController::getCommandLineOption(CommandLineParameters &cmdParams)
{
    return " -device mptsas1068,id=" + getId();
}

static const char xml_image[] = "Image";


DeviceStorage::DeviceStorage(const QString &n, Device *parent)
    : Device(n, parent)
{
}

void DeviceStorage::saveParameters(QXmlStreamWriter &xml) const
{
    Device::saveParameters(xml);
    xml.writeStartElement(xml_image);
    xml.writeCharacters(getImage());
    xml.writeEndElement();
}

void DeviceStorage::readParameters(QXmlStreamReader &xml)
{
    Device::readParameters(xml);
    xml.readNextStartElement();
    Q_ASSERT(xml.name() == xml_image);
    setImage(xml.readElementText());
}

QWidget *DeviceStorage::getEditorForm()
{
    return new DeviceStorageForm(this);
}

/******************************************************************************
 * IDE HDD                                                                    *
 ******************************************************************************/

const char DeviceIdeHd::typeName[] = "DeviceIdeHd";
const char DeviceIdeHd::deviceName[] = "IDE-HD";
REGISTER_DEVICE(DeviceIdeHd)

DeviceIdeHd::DeviceIdeHd()
    : DeviceStorage(deviceName, NULL)
{
    setRemovable(true);
}

DeviceIdeHd::DeviceIdeHd(const QString &img, Device *parent)
    : DeviceStorage(deviceName, parent)
{
    setRemovable(true);
}

QString DeviceIdeHd::getCommandLineOption(CommandLineParameters &cmdParams)
{
    DeviceBusIde *bus = dynamic_cast<DeviceBusIde*>(parent());
    Q_ASSERT(bus);
    DeviceIdeController *ide = dynamic_cast<DeviceIdeController*>(bus->parent());
    Q_ASSERT(ide);

    if (cmdParams.getLaunchMode() == LaunchMode::NORMAL)
    {
        QString cmdFile = " -drive file=" + getImage() + ",if=none,id="
            + getId() + "-file";
        return  cmdFile + " -device ide-hd"
            +",bus=" + ide->getId() + "." + QString::number(bus->getNumber()) 
            + ",drive=" + getId() + "-file"
            + ",id=" + getId();
    }
    else
    {
        QString overlay = cmdParams.getOverlayForImage(getImage());
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
const char DeviceIdeCdrom::deviceName[] = "IDE-CDROM";
REGISTER_DEVICE(DeviceIdeCdrom)

DeviceIdeCdrom::DeviceIdeCdrom()
    : DeviceStorage(deviceName, NULL)
{
    setRemovable(true);
}

DeviceIdeCdrom::DeviceIdeCdrom(const QString &img, Device *parent)
    : DeviceStorage(deviceName, parent)
{
    setRemovable(true);
}

QString DeviceIdeCdrom::getCommandLineOption(CommandLineParameters &cmdParams)
{
    DeviceBusIde *bus = dynamic_cast<DeviceBusIde*>(parent());
    Q_ASSERT(bus);
    DeviceIdeController *ide = dynamic_cast<DeviceIdeController*>(bus->parent());
    Q_ASSERT(ide);

    if (cmdParams.getLaunchMode() == LaunchMode::NORMAL)
    {
        QString cmdFile = " -drive file=" + getImage() + ",if=none,id="
            + getId() + "-file";
        return  cmdFile + " -device ide-cd"
            + ",bus=" + ide->getId() + "." + QString::number(bus->getNumber())
            + ",drive=" + getId() + "-file"
            + ",id=" + getId();
    }
    else
    {
        QString overlay = cmdParams.getOverlayForImage(getImage());
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


/******************************************************************************
* SCSI HDD                                                                    *
******************************************************************************/

const char DeviceScsiHd::typeName[] = "DeviceScsiHd";
const char DeviceScsiHd::deviceName[] = "SCSI-HD";
REGISTER_DEVICE(DeviceScsiHd)

DeviceScsiHd::DeviceScsiHd()
    : DeviceStorage(deviceName, NULL)
{
    setRemovable(true);
}

DeviceScsiHd::DeviceScsiHd(const QString &img, Device *parent)
    : DeviceStorage(deviceName, parent)
{
    setRemovable(true);
}

QString DeviceScsiHd::getCommandLineOption(CommandLineParameters &cmdParams)
{
    if (cmdParams.getLaunchMode() == LaunchMode::NORMAL)
    {
        QString cmdFile = " -drive file=" + getImage() + ",if=none,id="
            + getId() + "-file";
        return  cmdFile + " -device scsi-hd,drive=" + getId() + "-file";
    }
    else
    {
        QString overlay = cmdParams.getOverlayForImage(getImage());
        QString cmdFile = "-drive file=" + overlay + ",if=none,id="
            + getId() + "-file";

        return cmdFile + " -drive driver=blkreplay,if=none,image="
            + getId() + "-file,id=" + getId()
            + "-driver -device scsi-hd,drive=" + getId() + "-driver"
            + ",id=" + getId();
    }
}

bool DeviceScsiHd::isDeviceValid()
{
    return !getImage().isEmpty();
}

