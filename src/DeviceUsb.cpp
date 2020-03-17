#include "DeviceUsb.h"
#include "DeviceFactory.h"

// class DeviceUsbForm : public QGroupBox
// {
//     Q_OBJECT

// public:
//     DeviceUsbForm(DeviceUsb *dev);

// private:
//     DeviceUsb *device;

//     private slots:


// };


const char DeviceUsb::typeName[] = "DeviceUsb";


DeviceUsb::DeviceUsb(Device *parent) :
    Device("Usb", parent)
{
}

#ifdef GUI
QWidget *DeviceUsb::getEditorForm()
{
    return NULL;//new DeviceUsbForm(this);
}
#endif

QString DeviceUsb::getCommandLineOption()
{
    return "";
}

// DeviceUsbForm::DeviceUsbForm(DeviceUsb *dev)
//     : device(dev)
// {

// }


const char DeviceUsbEhci::typeName[] = "DeviceUsbEhci";

DeviceUsbEhci::DeviceUsbEhci(Device *parent) :
    Device("UsbEhci", parent)
{
}

#ifdef GUI
QWidget *DeviceUsbEhci::getEditorForm()
{
    return NULL;
}
#endif

QString DeviceUsbEhci::getCommandLineOption()
{
    return "-usb -device usb-ehci -device usb-host,vendorid=0x046d,productid=0x0829";
}



const char DeviceUsbXhci::typeName[] = "DeviceUsbEhci";

DeviceUsbXhci::DeviceUsbXhci(Device *parent) :
    Device("UsbEhci", parent)
{
}

#ifdef GUI
QWidget *DeviceUsbXhci::getEditorForm()
{
    return NULL;
}
#endif

QString DeviceUsbXhci::getCommandLineOption()
{
    return "-usb -device nec-usb-xhci -device usb-host,vendorid=0x046d,productid=0x0829";
}


