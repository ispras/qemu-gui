#ifndef DEVICESTORAGE_H
#define DEVICESTORAGE_H

#include "Device.h"

/*class DeviceStorage : public Device
{
public:
    DeviceStorage(const QString &n, Device *parent);
};*/

class DeviceStorageController : public Device
{
public:
    DeviceStorageController(const QString &n, Device *parent);
};

class DeviceIdeController : public DeviceStorageController
{
public:
    DeviceIdeController(Device *parent);
};

#endif // DEVICESTORAGE_H
