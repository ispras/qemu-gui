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
    DeviceStorageController() {}
    DeviceStorageController(const QString &n, Device *parent);

    virtual QString getDeviceTypeName() const = 0;
};

class DeviceIdeController : public DeviceStorageController
{
public:
    static const char typeName[];

    DeviceIdeController() { initDefault(); }
    DeviceIdeController(Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }

private:
    void initDefault();
};

#endif // DEVICESTORAGE_H
