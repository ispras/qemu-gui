#ifndef DEVICEUSB_H
#define DEVICEUSB_H

#include "Device.h"
#include "QemuGUICommon.h"


class DeviceUsb : public Device
{
public:
    static const char typeName[];

    DeviceUsb() {}
    DeviceUsb(Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual QWidget *getEditorForm();
    virtual QStringList getDeviceListToAdd() { return QStringList({"EHCI", "XHCI"}); }


protected:
    virtual QString getCommandLineOption();
};


class DeviceUsbForm : public QGroupBox
{
    Q_OBJECT

public:
    DeviceUsbForm(DeviceUsb *dev);

private:
    DeviceUsb *device;

    private slots:


};

class DeviceUsbEhci : public Device
{
public:
    static const char typeName[];

    DeviceUsbEhci() {}
    DeviceUsbEhci(Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual QWidget *getEditorForm();


protected:
    virtual void saveParameters(QXmlStreamWriter &xml) const;
    virtual void readParameters(QXmlStreamReader &xml);
    virtual QString getCommandLineOption();
};

class DeviceUsbXhci : public Device
{
public:
    static const char typeName[];

    DeviceUsbXhci() {}
    DeviceUsbXhci(Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual QWidget *getEditorForm();


protected:
    virtual void saveParameters(QXmlStreamWriter &xml) const;
    virtual void readParameters(QXmlStreamReader &xml);
    virtual QString getCommandLineOption();
};


#endif // DEVICEUSB_H
