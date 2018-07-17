#include "DeviceStorage.h"
#include "DeviceBus.h"

DeviceStorageController::DeviceStorageController(const QString &n, Device *parent)
    : Device(n, parent)
{

}

const char DeviceIdeController::typeName[] = "DeviceIdeController";

DeviceIdeController::DeviceIdeController(Device *parent)
    : DeviceStorageController("IDE", parent)
{
    initDefault();
}

void DeviceIdeController::initDefault()
{
    new DeviceBusIde(0, this);
    new DeviceBusIde(1, this);
}

DeviceStorage::DeviceStorage(const QString &n, Device *parent)
    : Device(n, parent)
{

}

const char DeviceIdeHd::typeName[] = "DeviceIdeHd";

DeviceIdeHd::DeviceIdeHd(Device *parent)
    : DeviceStorage("IDE-HD", parent)
{
}
