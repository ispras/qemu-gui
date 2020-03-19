#ifndef DEVICEUSB_H
#define DEVICEUSB_H

#include "Device.h"

class DeviceUsb : public Device
{
public:
    static const char typeName[];

    DeviceUsb() {}
    DeviceUsb(Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
#ifdef GUI
    virtual QWidget *getEditorForm();
#endif

protected:
    virtual QString getCommandLineOption();
};

class DeviceUsbEhci : public Device
{
public:
    static const char typeName[];

    DeviceUsbEhci() {}
    DeviceUsbEhci(Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
#ifdef GUI
    virtual QWidget *getEditorForm();
#endif

protected:
    virtual QString getCommandLineOption();
};

class DeviceUsbXhci : public Device
{
public:
    static const char typeName[];

    DeviceUsbXhci() {}
    DeviceUsbXhci(Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
#ifdef GUI
    virtual QWidget *getEditorForm();
#endif

protected:
    virtual QString getCommandLineOption();
};


#endif // DEVICEUSB_H
