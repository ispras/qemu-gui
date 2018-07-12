#ifndef DEVICEBUS_H
#define DEVICEBUS_H

#include "Device.h"

class DeviceStorageController;

class DeviceBus : public Device
{
public:
    DeviceBus(const QString &n, Device *parent);
};

class DeviceBusIde : public DeviceBus
{
public:
    DeviceBusIde(int id, DeviceIdeController *parent);
};

#endif // DEVICEBUS_H
