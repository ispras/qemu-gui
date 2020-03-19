#include "DeviceStorage.h"
#include "DeviceBus.h"
#include "DeviceFactory.h"
#include "CommandLineParameters.h"
#ifdef GUI
#include "DeviceForm.h"
#endif

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
}

DeviceIdeController::DeviceIdeController(Device *parent)
    : DeviceStorageController(deviceName, parent)
{
    initDefault();
}

QString DeviceIdeController::getDeviceInfo()
{
    return "IDE devices: \n";
}

void DeviceIdeController::initDefault()
{
    (new DeviceBusIde(0, this))->setRemovable(false);
    (new DeviceBusIde(1, this))->setRemovable(false);
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

static const char xml_controller[] = "Controller";
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

#ifdef GUI
QWidget *DeviceScsiController::getEditorForm()
{
    return new DeviceScsiControllerForm(this);
}
#endif

const QStringList &DeviceScsiController::getControllers() const
{
    static QStringList controllers = {"mptsas1068", "lsi53c810", "lsi53c895a",
        "megasas", "megasas-gen2", "am53c974", "dc390"};
    return controllers;    
}

void DeviceScsiController::initDefault()
{
    setId("scsi");
    controller = getControllers().first();
}

QString DeviceScsiController::getCommandLineOption(CommandLineParameters &cmdParams)
{
    return " -device " + controller + ",id=" + getId();
}

void DeviceScsiController::saveParameters(QXmlStreamWriter &xml) const
{
    xml.writeStartElement(xml_controller);
    xml.writeCharacters(controller);
    xml.writeEndElement();
}

void DeviceScsiController::readParameters(QXmlStreamReader &xml)
{
    xml.readNextStartElement();
    Q_ASSERT(xml.name() == xml_controller);
    controller = xml.readElementText();
}

QString DeviceScsiController::getDeviceInfo()
{
    return "SCSI controller: " + controller + "\n";
}


static const char xml_image[] = "Image";
static const char xml_cmdLine[] = "CmdLineOption";

DeviceStorage::DeviceStorage(const QString &n, Device *parent)
    : Device(n, parent)
{
}

void DeviceStorage::saveParameters(QXmlStreamWriter &xml) const
{
    xml.writeStartElement(xml_image);
    xml.writeCharacters(getImage());
    xml.writeEndElement();
}

void DeviceStorage::readParameters(QXmlStreamReader &xml)
{
    xml.readNextStartElement();
    Q_ASSERT(xml.name() == xml_image);
    setImage(xml.readElementText());
}

#ifdef GUI
QWidget *DeviceStorage::getEditorForm()
{
    return new DeviceStorageForm(this);
}
#endif

/******************************************************************************
 * IDE HDD                                                                    *
 ******************************************************************************/

const char DeviceIdeHd::typeName[] = "DeviceIdeHd";
const char DeviceIdeHd::deviceName[] = "IDE-HD";
REGISTER_DEVICE(DeviceIdeHd)

DeviceIdeHd::DeviceIdeHd()
    : DeviceStorage(deviceName, NULL)
{
}

DeviceIdeHd::DeviceIdeHd(const QString &img, Device *parent)
    : DeviceStorage(deviceName, parent)
{
    setImage(img);
}

QString DeviceIdeHd::getCommandLineOption(CommandLineParameters &cmdParams)
{
    DeviceBusIde *bus = dynamic_cast<DeviceBusIde*>(parent());
    Q_ASSERT(bus);
    DeviceIdeController *ide = dynamic_cast<DeviceIdeController*>(bus->parent());
    Q_ASSERT(ide);

    if (cmdParams.getLaunchMode() == LaunchMode::NORMAL)
    {
        QString cmdFile = " -drive file=\"" + getImage() + "\"" + ",if=none,id="
            + getId() + "-file";
        return  cmdFile + " -device ide-hd"
            +",bus=" + ide->getId() + "." + QString::number(bus->getNumber()) 
            + ",drive=" + getId() + "-file"
            + ",id=" + getId();
    }
    else
    {
        QString overlay = cmdParams.getOverlayForImage(getImage());
        QString cmdFile = " -drive file=\"" + overlay + "\"" + ",if=none,id="
            + getId() + "-file";
        QString overlayEnabled = cmdParams.isOverlayEnabled() ? "" : ",snapshot=on";

        return cmdFile + overlayEnabled + " -drive driver=blkreplay,if=none,image="
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

QString DeviceIdeHd::getDeviceInfo()
{
    DeviceBusIde *bus = dynamic_cast<DeviceBusIde*>(parent());
    Q_ASSERT(bus);
    return "\tHard Disk:\n\t\tImage: " + getImage() + "\n"
        + "\t\tBus: " + "ide." + QString::number(bus->getNumber()) + "\n";
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
}

DeviceIdeCdrom::DeviceIdeCdrom(const QString &img, Device *parent)
    : DeviceStorage(deviceName, parent)
{
    setImage(img);
}

QString DeviceIdeCdrom::getCommandLineOption(CommandLineParameters &cmdParams)
{
    DeviceBusIde *bus = dynamic_cast<DeviceBusIde*>(parent());
    Q_ASSERT(bus);
    DeviceIdeController *ide = dynamic_cast<DeviceIdeController*>(bus->parent());
    Q_ASSERT(ide);

    if (cmdParams.getLaunchMode() == LaunchMode::NORMAL)
    {
        QString cmdFile = " -drive file=\"" + getImage() + "\"" + ",if=none,id="
            + getId() + "-file";
        return  cmdFile + " -device ide-cd"
            + ",bus=" + ide->getId() + "." + QString::number(bus->getNumber())
            + ",drive=" + getId() + "-file"
            + ",id=" + getId();
    }
    else
    {
        QString overlay = cmdParams.getOverlayForImage(getImage());
        QString cmdFile = " -drive file=\"" + overlay + "\"" + ",if=none,id="
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

QString DeviceIdeCdrom::getDeviceInfo()
{
    DeviceBusIde *bus = dynamic_cast<DeviceBusIde*>(parent());
    Q_ASSERT(bus);
    return "\tCD-ROM:\n\t\tImage: " + getImage() + "\n"
        + "\t\tBus: " + "ide." + QString::number(bus->getNumber()) + "\n";
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
}

DeviceScsiHd::DeviceScsiHd(const QString &img, Device *parent)
    : DeviceStorage(deviceName, parent)
{
    setImage(img);
}

QString DeviceScsiHd::getCommandLineOption(CommandLineParameters &cmdParams)
{
    // TODO: bus?
    DeviceScsiController *scsi = dynamic_cast<DeviceScsiController *>(parent());
    Q_ASSERT(scsi);
    if (cmdParams.getLaunchMode() == LaunchMode::NORMAL)
    {
        QString cmdFile = " -drive file=\"" + getImage() + "\"" + ",if=none,id="
            + getId() + "-file";
        return  cmdFile + " -device scsi-hd,drive=" + getId() + "-file"
            + ",bus=" + scsi->getId() + ".0";
    }
    else
    {
        QString overlay = cmdParams.getOverlayForImage(getImage());
        QString cmdFile = " -drive file=\"" + overlay + "\"" + ",if=none,id="
            + getId() + "-file";

        return cmdFile + " -drive driver=blkreplay,if=none,image="
            + getId() + "-file,id=" + getId()
            + "-driver -device scsi-hd,drive=" + getId() + "-driver"
            + ",bus=" + scsi->getId() + ".0" + ",id=" + getId();
    }
}

bool DeviceScsiHd::isDeviceValid()
{
    return !getImage().isEmpty();
}

QString DeviceScsiHd::getDeviceInfo()
{
    DeviceScsiController *scsi = dynamic_cast<DeviceScsiController *>(parent());
    Q_ASSERT(scsi);
    return "\tImage: " + getImage() + "\n"
        + "\tBus: " + scsi->getId() + ".0" + "\n";
}

