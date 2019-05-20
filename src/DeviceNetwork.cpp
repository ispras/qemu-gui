#include "DeviceNetwork.h"
#include "DeviceFactory.h"
#include "DeviceForm.h"
#include "CommandLineParameters.h"


const char DeviceNetworkController::typeName[] = "DeviceNetworkController";
const char DeviceNetworkController::deviceName[] = "Network adapter";

static const char xml_controller[] = "Controller";
static const char xml_netdev[] = "NetworkDevice";
static const char xml_tapIfName[] = "TapIfName";

static const char noNetdev[] = "unplugged";
REGISTER_DEVICE(DeviceNetworkController)


DeviceNetworkController::DeviceNetworkController()
    : Device(deviceName, NULL)
{
    initDefault();
}

DeviceNetworkController::DeviceNetworkController(const QString &n, Device *parent)
    : Device(deviceName, parent)
{
    initDefault();
}

void DeviceNetworkController::initDefault()
{
    controller = getControllers().first();
    netdev = getNetdevBackend().first();
}

QWidget *DeviceNetworkController::getEditorForm()
{
    return new DeviceNetworkForm(this);
}

const QStringList &DeviceNetworkController::getControllers() const
{
    static QStringList controllers = { "rtl8139", "e1000", "i82550", "ne2k_pci",
                                       "pcnet", "virtio-net-pci", "vmxnet3" };
    return controllers;
}

const QStringList &DeviceNetworkController::getNetdevBackend() const
{
    static QStringList netdev = { "user", "tap", noNetdev };
    return netdev;
}

QString DeviceNetworkController::getCommandLineOption(CommandLineParameters &cmdParams)
{
    if (netdev.compare(noNetdev) != 0)
    {
        QString netdevCmd = " -netdev " + netdev + ",id=" + getId();
        QString devCmd = " -device " + controller + ",netdev=" + getId();
        QString tapCmd = netdevCmd + ",ifname=" + tapIfName +
            ",script=no,downscript=no" + devCmd;
        QString netAllCmd = netdevCmd + devCmd;
        QString rrCmd = " -object filter-replay,id=replay,netdev=" + getId();
        if (cmdParams.getLaunchMode() == LaunchMode::NORMAL)
        {
            if (netdev.compare("tap") != 0)
            {
                return netAllCmd;
            }
            else
            {
                return tapCmd;
            }
        }
        else
        {
            if (netdev.compare("tap") != 0)
            {
                return netAllCmd + rrCmd;
            }
            else
            {
                return tapCmd + rrCmd;
            }
        }
    }
    else
    {
        return " -device " + controller;
    }
}

void DeviceNetworkController::saveParameters(QXmlStreamWriter &xml) const
{
    xml.writeStartElement(xml_controller);
    xml.writeCharacters(controller);
    xml.writeEndElement();

    xml.writeStartElement(xml_netdev);
    xml.writeCharacters(netdev);
    xml.writeEndElement();

    xml.writeStartElement(xml_tapIfName);
    xml.writeCharacters(tapIfName);
    xml.writeEndElement();
}

void DeviceNetworkController::readParameters(QXmlStreamReader &xml)
{
    xml.readNextStartElement();
    Q_ASSERT(xml.name() == xml_controller);
    controller = xml.readElementText();

    xml.readNextStartElement();
    Q_ASSERT(xml.name() == xml_netdev);
    netdev = xml.readElementText();

    xml.readNextStartElement();
    Q_ASSERT(xml.name() == xml_tapIfName);
    tapIfName = xml.readElementText();
}

QString DeviceNetworkController::getDeviceInfo()
{
    return "Network:\n\tController: " + controller + "\n"
        + "\tBackend type: " + netdev + "\n";
}


