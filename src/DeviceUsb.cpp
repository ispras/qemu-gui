#include "DeviceUsb.h"
#include "DeviceFactory.h"

const char DeviceUsb::typeName[] = "DeviceUsb";

DeviceUsb::DeviceUsb(Device *parent) :
    Device("Usb", parent)
{
}

QWidget *DeviceUsb::getEditorForm()
{
    return NULL;//new DeviceUsbForm(this);
}

QString DeviceUsb::getCommandLineOption()
{
    return "";
}

DeviceUsbForm::DeviceUsbForm(DeviceUsb *dev)
    : device(dev)
{

}


const char DeviceUsbEhci::typeName[] = "DeviceUsbEhci";

DeviceUsbEhci::DeviceUsbEhci(Device *parent) :
    Device("UsbEhci", parent)
{
}

QWidget *DeviceUsbEhci::getEditorForm()
{
    return NULL;
}

void DeviceUsbEhci::saveParameters(QXmlStreamWriter &xml) const
{
}

void DeviceUsbEhci::readParameters(QXmlStreamReader &xml)
{
}

QString DeviceUsbEhci::getCommandLineOption()
{
    return "-usb -device usb-ehci -device usb-host,vendorid=0x046d,productid=0x0829";
}



const char DeviceUsbXhci::typeName[] = "DeviceUsbEhci";

DeviceUsbXhci::DeviceUsbXhci(Device *parent) :
    Device("UsbEhci", parent)
{
}

QWidget *DeviceUsbXhci::getEditorForm()
{
    return NULL;
}

void DeviceUsbXhci::saveParameters(QXmlStreamWriter &xml) const
{
}

void DeviceUsbXhci::readParameters(QXmlStreamReader &xml)
{
}

QString DeviceUsbXhci::getCommandLineOption()
{
    return "-usb -device nec-usb-xhci -device usb-host,vendorid=0x046d,productid=0x0829";
}


