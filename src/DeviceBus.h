#ifndef DEVICEBUS_H
#define DEVICEBUS_H

#include "Device.h"

class DeviceStorageController;
class DeviceIdeController;

class DeviceBus : public Device
{
public:
    DeviceBus(const QString &n, Device *parent);

    virtual QString getDeviceTypeName() const = 0;
};

class DeviceBusIde : public DeviceBus
{
public:
    static const char typeName[];

    DeviceBusIde(int id, DeviceIdeController *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual QStringList getDeviceListToAdd() { return QStringList({ "IDE-HD" }); }
};

#endif // DEVICEBUS_H
